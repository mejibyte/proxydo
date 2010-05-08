/* 
   Downloaded from http://www.steveshipway.org/software/utils/www-proxy.c
 */

/* www-proxy.c  Copyright Imperial Business Systems 1995
   Proxy http server.
   Written by Steve Shipway (steve@cheshire.demon.co.uk,steve@imperial.co.uk)
   in the space of 2 hours.  It took a further 1 hour to remove the last
bug.  And 2 hours to persuade myself to stop playing with Netscape after it
was working.

	Command line args:
		-d : debug mode.  Use port 8000 instead, dont background, status msgs
		-l logfile  : logfile to use
		-p port : specify different port number

	The port to use is:   
		specified on command line, or
		http-proxy in /etc/services, or
		800

	The destination default port is
		http in /etc/services, or
		80

	This is provided as-is, with no warrenty, expressed or implied.
	This source code remains the property of imperial business systems
	(ibs@imperial.co.uk)
	Not to be distributed for profit, or indeed at all, without permission.

	Fan mail to steve@imperial.co.uk

	This works under AIX and SCO, you may need slight mods (especially 
	the wait3() system call and header files) on other machines. YMMV.

	NOTE that you cant bind to ports under 1024 unless you are root.
	Therefore, if you dont have root access, use -p8000 or similar and
	run the proxy on port 8000, rather than 800 (the usual proxy port)
*/

#define SCO

/* MAKE SURE THAT YOU HAVE :

#define SCO
   if running SCO unix 3.2 or above, and

#define AIX
   if running AIX 3.4

   I dont know about AIX 4, or any other flavours, but they should be
   relatively likely to compile.
*/

#include <stdio.h>
#include <stdlib.h>
#ifdef AIX
#include <sys/time.h>
#endif
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <netdb.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/resource.h>
#ifdef AIX
#include <sys/m_wait.h>
#endif
#ifdef SCO
#define __SCO_WAIT3__
#endif
#include <sys/wait.h>

#define NONE (fd_set *) NULL
#define NEVER (struct timeval *) NULL
#define IGNORE (struct sockaddr *) NULL

#define PORTTEST 8000

int debug = 0;
char buffer[4096];
int httpport;
int s;
fd_set active;
struct sockaddr_in sc_in;
FILE *lfp = NULL;
char **info;

/*********************************************/
fdsend(int fd,char *title, char *head, char *body)
{
	sprintf(buffer,"<HEAD><TITLE>HTTP Proxy Server Message</TITLE></HEAD>\n<BODY>\n");
	write(fd,buffer,strlen(buffer));
	if(title) {
		sprintf(buffer,"<H1>%s</H1>\n",title);
		write(fd,buffer,strlen(buffer));
	}
	if(head) {
		sprintf(buffer,"<H2>%s</H2>\n",head);
		write(fd,buffer,strlen(buffer));
	}
	if(body) {
		sprintf(buffer,"%s<P>\n",body);
		write(fd,buffer,strlen(buffer));
	}
	sprintf(buffer,"</BODY>\n");
	write(fd,buffer,strlen(buffer));
}

fdserve(int fd)
{
	/* serve requests on fd */
	char buf2[512];
	struct sockaddr_in sin;
	int rfd; /* request destination */
	int n;
	fd_set rfds;
	struct hostent *shes;
	char *url;
	char host[128];
	char *h,*rest;
	int portnum;
	struct timeval tv;	
	extern int h_errno;
	char cmd[16];
	int i;

	if(debug) printf("* Spawning **************************\n");
	
	n=read(fd,buffer,sizeof(buffer));
	if(n<1) {
		if(lfp) fprintf(lfp,"*** read() failed in child process.***\n");
		if(debug) {
			if(n<0) perror("read");
			printf("Nothing read.\n");
		}
		return;
	}
	FD_ZERO(&rfds);
	FD_SET(fd,&rfds);

	for(i=0;i<15;i++)
		if(buffer[i] && (buffer[i]!=' ')) {
			cmd[i]=(char)toupper((int)buffer[i]);
		} else break;
	cmd[i]='\0';
	if(strcmp(cmd,"GET")&&strcmp(cmd,"POST"))  {
		if(debug) printf("Not a GET or POST command\n");
		if(lfp) { fprintf(lfp,"Bad command [%s] received.\n---------\n",cmd);
		fprintf(lfp,"%s\n----------\n",buffer);fflush(lfp); }
		fdsend(fd,"Error 405",cmd,"This command is not supported by the proxy server.");
		close(fd);
		return;
	}	

	/* identify where to go to */
	/* GET http://hostname[:port][blurfl....]\n */
	for(url=buffer;*url && (*url!='/');url++);
	url+=2;
	h=host;
	(*info)=host;
	for(;*url&&(*url!=':')&&(*url!='/');url++) *(h++)=*url;
	*h='\0';
	if(*url==':') {
		portnum = atoi(url+1);
		for(;*url!='/';url++);
	} else {
		portnum = httpport;
	}
	for(rest=url;*rest && (*rest!='\n');rest++);
	if(*rest){ *rest='\0'; rest++; }

	if(debug) printf("%s http://%s:%d%s\n",cmd,host,portnum,url);
	if(lfp) { fprintf(lfp,"%s http://%s:%d%s\n",cmd,host,portnum,url);
		fflush(lfp); }

	/* connect to server */
	if ((rfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		if(debug) perror("socket");
		if(lfp) fprintf(lfp,"*** socket() failed in child process. ***\n");
		return 0;
	}
	if(debug>2) printf("rfd is on fd %d\n",rfd);
	
	if(debug) {
		printf("gethostbyname(%s)=",host);
	}
	shes = gethostbyname(host);
	if(debug) {
		if(!shes) {
			printf("null\n");
		}else{
			printf("%d.%d.%d.%d\n",
				(unsigned char)shes->h_addr[0],
				(unsigned char)shes->h_addr[1],
				(unsigned char)shes->h_addr[2],
				(unsigned char)shes->h_addr[3]
			);
		}
	}
	if(!shes) {
		if(debug) printf("Host %s not found\n",host);
		if(lfp){ fprintf(lfp,"Host %s was not found.\n",host);
			fflush(lfp); }
		switch(h_errno) {
			case HOST_NOT_FOUND:
				sprintf(buf2,"Sorry, this host could not be identified by the DNS server.\nMaybe the address is wrong or mistyped.");
				break;
			case TRY_AGAIN:
				sprintf(buf2,"Sorry, there is a temporary problem with the DNS server.  Please try again.");
				break;
			case NO_DATA:
				sprintf(buf2,"Sorry, this is a virtual host without an IP address.");
				break;
			default:
				sprintf(buf2,"Unable to resolve this host name to an IP address.");
		}
		fdsend(fd,"Sorry!",host,buf2);
		close(fd);
		if(lfp) {fprintf(lfp,"** Host %s not found.\n",host);
			fflush(lfp); }
		return 0;
	}
	if(debug) printf("Trying %s on port %d...\n",host,
		 portnum);
	sin.sin_family = AF_INET;
	bcopy(shes->h_addr,&sin.sin_addr,sizeof(struct in_addr)) ;
	sin.sin_port = htons((u_short) portnum) ;
	if(connect(rfd, (struct sockaddr *) &sin, sizeof(struct sockaddr_in)) < 0) {
		if(debug) perror("connect");
		if(lfp) { fprintf(lfp,"Connect failed to host %s [%d]\n",host,errno);
			fflush(lfp); }
		switch(errno) {
			case ECONNREFUSED:
				sprintf(buf2,"Sorry, this host is refusing connections from this machine on the specified port.  This is possibly due to the host not running a Web server, or an incorrect port number being specified.\n");
				break;
			case EHOSTUNREACH:
			case ENETUNREACH:
				sprintf(buf2,"Sorry, this host is currently unreachable from this network.\n");
				break;
			case EHOSTDOWN:
			case ENETDOWN:
				sprintf(buf2,"Sorry, this host is down.  Try again later.\n");
				break;
			case ETIMEDOUT:
				sprintf(buf2,"Sorry, the connection to this host was timed out.  This is most likely due to network overload.  Try again later.\n");
				break;
			default:
				sprintf(buf2,"Sorry, this host is not responding.  This may be due to either the remote host being down, to 'netlag' (slow connections), or to an incorrect port number in the URL.");
		}
		fdsend(fd,"Sorry!",host,buf2);
		close(fd);
		return 0;
	}

	if(debug) printf("* Connected to %s on port %d\n",host,portnum);
	if(lfp) { 
		fprintf(lfp,"Connected to %s:%d\n",host,portnum);	
		fflush(lfp);
	}

	sprintf(buf2,"%s %s\n",cmd,url);
	if(write(rfd,buf2,strlen(buf2))<1) {
		perror("write[a]");
		close(fd);
		close(rfd);
		return 0;
	}
	if(strlen(rest)>0) 
		if(write(rfd,rest,strlen(rest) )<1){
			perror("write[b]");
			close(fd);
			close(rfd);
			return 0;
		}

	/* phew. now send anything back and forth until we run out. */
	for(;;) {
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		FD_SET(rfd,&rfds);
		FD_SET(fd,&rfds);
		if (select(FD_SETSIZE, &rfds, NONE, NONE, &tv) < 0)  {
			
			if(debug) { 
				perror("select"); puts("timeout");	
			}
			break;
		}
		if( FD_ISSET( rfd, &rfds ) ) {
			/* data coming in */
			if(debug) printf("+");
			if((n=read(rfd,buffer,sizeof(buffer)))<1) break;
			if(write(fd,buffer,n)<n) break;
		} else {
			/* data going out */
			if(debug) printf("-");
			if((n=read(fd,buffer,sizeof(buffer)))<1) break;
			if(write(rfd,buffer,n)<n) break;
		}
	}

	if(lfp) { fprintf(lfp,"Transfer complete.\n"); fflush(lfp); }
	close(rfd);
	close(fd);
}

main(int argc, char *argv[])
{
	int j;
	int children= 0;
	int uw;
	struct timeval tv;
	extern int optind;
	extern char *optarg;
	int c,port;
	char statbuf[32];
	struct servent *sss;
	char *logfile=(char *)0;
	int last;

	info=argv+1;
	port=0;
	while((c=getopt(argc,argv,"dp:l:"))!=-1) 
		switch(c) {
			case 'd': debug++; break;
			case 'p': port=atoi(optarg); break;
			case 'l': logfile = optarg; break;
			default: printf("Usage: %s [-d] [-p port] [-l logfile]\n",argv[0]);
				exit(1);
		}


	if(logfile) {
		lfp=fopen(logfile,"a");
		if(!lfp) perror(logfile);
	}

	if(!port) {
		sss = getservbyname("http-proxy","tcp");
		port=(debug?PORTTEST:(sss?(u_short)htons(sss->s_port):800));
	}
	if(debug && sss) printf("Using proxy port [%-10.10s] %d\n",sss->s_name,port);
	argv[1]=statbuf;

/*	signal(SIGCHLD,wait); */
	signal(SIGHUP,SIG_IGN); 

	sss = getservbyname("http","tcp");
	httpport=(sss?(u_short)htons(sss->s_port):80);
	if(debug) printf("Using http port [%-10.10s] %d\n",sss->s_name,httpport);

	/* open port to listen to */
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		if(debug) perror("socket");
		return 0;
	}
	sc_in.sin_family = AF_INET;
	sc_in.sin_addr.s_addr = INADDR_ANY;
	sc_in.sin_port = htons((u_short) port);
	if (bind(s, (struct sockaddr *) &sc_in, sizeof(sc_in)) < 0) {
		perror("bind: proxy");
		if(lfp) fprintf(lfp,"bind failed.\n");
		return 0;
	}
	if (listen(s, 25) < 0) {
		perror("listen");
		return 0;
	}

	/* now try and set the socket option to allow reuse of address */
	{
		int b;
		b=-1;
		if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&b,4)) {
			perror("setsockopt");
			if(lfp) fprintf(lfp,"setsockopt failed.\n");
			exit(1);
		}
	}

	/* set up fd mask */
	FD_ZERO(&active);
	FD_SET(s,&active);

	/* dont setsid if in debug mode */
	if(!debug)  {
		if( fork() ) exit(0);
		setsid(); /* this function may not exist on all UNIXs. */
		/* if you dont have setsid(), then do the following:
		close all fds, set process group this getpid(), use
		ioctl to set NOTTY on stdin. This latter is not *vital*,
		everything should still work anyway.*/
	}

	if(lfp) { fprintf(lfp,"*** Program starting.\n"); fflush(lfp); }
	/* SELECT LOOP */
	/* we loop until something happens! */
	children=0;
	last = s;
	for( ;; ) {
		sprintf(statbuf,"port %d, %d procs, last=%d" ,port,children,last);
		if(debug) printf("** Waiting..\n");
		while(wait3(&uw,WNOHANG,NULL)>0) {
			children--;
			if(debug) printf("** Removed zombie\n");
		}
		if(debug) printf("** Selecting..\n");
		tv.tv_sec = 60;
		tv.tv_usec = 0;
		FD_ZERO(&active);
		FD_SET(s,&active);
		if (select(FD_SETSIZE, &active, NONE, NONE, &tv) < 0) {
			if(debug)
			perror("select");
			if(lfp) fprintf(lfp,"*** SELECT FAILED ***\n");
			break;
		}
		if( FD_ISSET(s,&active) ) {
			if ((j = accept(s, IGNORE, (int *) 0)) < 0) {
				if(debug) perror("accept");
				if(lfp) { fprintf(lfp,"* accept() failed\n"); fflush(lfp);}
			} else {
				last=j;
				children++;
				if(!fork()) {
					close(s);
					fdserve(j);
					if(debug) printf("* Child exiting.\n");
					exit(0);
				}
				close(j);
			}
		}
	}
	if(debug) printf("** Bye.\n");
	if(lfp) fprintf(lfp,"***** EXITING PROGRAM *****\n");
	exit(0);
}
