/*stream client:echo what is received from client*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <netdb.h>
#include <sys/fcntl.h>

struct hostent *gethostbyname(const char *name);
struct argumentos{
	int tthread;
	int t;
  	u_short sin_port;//16 bit num, network byte order
  	struct in_addr sin_addr;//32 bits host address
};
 
struct argumentos argumentos_array[5];
 
void getHostname(void){
	
	
	
}
void *escuhador(void *argos){
	int n2,t;
	struct sockaddr_in sin2;
	struct hostent *hptr2;
	struct argumentos *a = argos;
	int s;
	t = a->t;
	char msg[4000];
	printf("[THREAD] %d\n",a->tthread);
  	puts("[INFO] Conexion Recibida y Aceptada...");
  	printf("[IP ENTRANTE] %s [PUERTO ENTRANTE] %d\n",inet_ntoa(a->sin_addr),ntohs(a->sin_port));
  	puts("[INFO] Leyendo Mensaje de cliente...");
 

	if(read(a->t,msg,sizeof(msg))<0){//Read message from client
		puts("[ERROR]");
      	perror("read");
		return -1;
    }
  	puts("[INFO] Mensaje Leido..");
    printf("[MENSAJE] %s \n",msg);
	puts("[INFO] Interpretando...");
	puts("[INFO] Creando socket Client/Servidor...");	
	
	if((s=socket(AF_INET, SOCK_STREAM, 0))<0){//create socket
		puts("[ERROR]");
      	perror("socket");//socket error
		return -1;
    }
    
	
    sin2.sin_family = AF_INET;//set portocol family to internet
    sin2.sin_port = htons(atoi("80"));//set port number
    
    //puerto q se le asigna al sin_port    printf("port %d\n",sin.sin_port);
  	puts("[INFO] Verificando direccion destino...");	
   	if ((hptr2 = gethostbyname("circusoflife.com"))== NULL){
      	fprintf(stderr,"[ERROR] gethostname error: %s\n","circusoflife");
		return -1;
    }
    
    memcpy(&sin2.sin_addr,hptr2->h_addr,hptr2->h_length);
    
  	printf("[INFO] Conectando con Servidor...\n") ; 
   	if(connect(s,(struct sockaddr *)&sin2,sizeof(sin2))<0){
		puts("[ERROR]");
      	perror("connect");
		return -1;
    }

	

  	printf("[INFO] Conectado con exito...\n");
  	printf("[INFO] Enviando mensaje al servidor...\n");
  	if(write(s,msg,strlen(msg)+1)<0){ //send messeage to server
		puts("[ERROR]");
      	perror("write");
		return -1;
    }
	
	printf("[INFO] Mensaje Enviado...\n");
   	printf("[INFO] Leer mensaje desde servidor...\n");
	

		
		if((read(s,msg,sizeof(msg)))<0){//read message form server
				printf("[ERROR] ");
		  		perror("read");
				return -1;
			}

			printf("\nMENSAJE: %d\n",strlen(msg));
			printf("[MENSAJE] %s \n",msg);
			if(write(t,msg,strlen(msg))<0){
				puts("ERROR EN WRITE");
				return -1;	
			}
			puts("MENSJAE ENVIADO");
			
			
		

	
	
	
    puts("[MENSAJE ENVIADO]");
    puts("[CERRANDO CONEXION]");
    if(close(s)<0){
		printf("[ERROR] ");
      	perror("close");//close error
		return -1;
	}
	
	if(close(t)<0){
		printf("[ERROR] ");
      	perror("close");
		return -1;
    }
 	printf("[FIN] thread: %d\n",pthread_self()/*a->tthread*/);
    printf("\n========================\n");
	
}

int main(int argc, char *argv[]){
  	int num_threads = 5;
  	int threads_count=0;
  	int s,t,sinlen,error;
  	struct sockaddr_in sin;
  	struct hostent *hptr;
  	pthread_t threads[num_threads];
	pthread_t usando[num_threads];
  	char msg[1500];
  	if(argc <2){
    	printf("%s port\n",argv[0]);//input error:need port number!
    	return -1;
  	}
  	printf("[INFO] Creando socket...\n");
  	if((s=socket(AF_INET,SOCK_STREAM,0))<0){//create socket
    	perror("socket");//socket error
		exit(1);
    	return -1;
  	}
  
  	sin.sin_family = AF_INET;//set portocol family to internet
  	sin.sin_port = htons(atoi(argv[1]));//set port number
  	sin.sin_addr.s_addr = INADDR_ANY;//set ip address to any interface
 	puts("[INFO] Estableciendo la conexion con el cliente...");
  	if(bind(s,(struct sockaddr *)&sin,sizeof(sin))<0){
    	perror("bind");//bind error
    	return -1;
  	}
  	puts("[INFO] Conexion establecida...");
  	puts("[INFO] Servidor esta en estado LISTEN...");
  	//server indicates it's ready, max. listed queus is 5
  	if(listen(s,5)){
		puts("[ERROR]");
    	perror("listen"); //listen error
		exit(1);
    	return -1;
  	}
	int t1 = 0;
  	sinlen = sizeof(sin);
	for(;;){
    	puts("[INFO] Servidor entra a estado LISTEN...");
    //accepting new connection request from cl	ient,
    //socket id from the new connection is returned in t

		
    	puts("[INFO] Esperando nueva peticion de conexion del cliente...");
    	if((t = accept(s,(struct sockaddr *) &sin,(void *)&sinlen))<0){
			puts("[ERROR]");
      		perror("accept"); //accept error
			//break;
    	}

		printf("[[T]]:  %d\n",t);
		argumentos_array[threads_count].t = t;
		argumentos_array[threads_count].sin_port = sin.sin_port;
		argumentos_array[threads_count].sin_addr.s_addr = sin.sin_addr.s_addr;
		argumentos_array[threads_count].tthread = threads_count;
    	if(error = pthread_create(&threads[threads_count],NULL,escuhador,(void *) &argumentos_array[threads_count])==-1)
			perror((void *)error);	
		threads_count++;
		if(threads_count == num_threads)
			threads_count = 0;
		
		/*
		printf("[INFO] Conexion Recibida...\n");
	    printf("[INFO] Conexion Aceptada..\n");
	    printf("[IP ENTRANTE] %s [PUERTO ENTRANTE] %d\n",inet_ntoa(sin.sin_addr),ntohs(sin.sin_port));

	    printf("[INFO] Leyendo Mensaje de cliente..\n");
	    if(read(t,msg,sizeof(msg))<0){//Read message from client
	      perror("read");
	      return -1;
	    }

	    printf("[INFO] Mensaje Leido..\n");
	    printf("[MENSAJE] %s \n",msg);
	    printf("[INFO] Reenviando Mensaje..\n");

	    if(write(t,msg2,strlen(msg2))<0){//echo message back
	      perror("write");
	      return -1;
	    }

	    printf("[MENSAJE ENVIADO]\n");
	    printf("[CERRANDO CONEXION]\n");
	    //close connection cleanup suckets
	    if(close(t)<0){
	      perror("close");
	      return -1;
	    }
	    printf("[CONEXION CERRADA]\n");
	    printf("\n===========================\n");
			
	*/
	

  }
  //not reach below
  printf("[EROR] CERANDO CONEXION\n");
  if(close(s)<0){
 	perror("close");
    return -1;
  }
  
  return 0;
}

 /*
  struct hostent{
  char *h_name;       //canonical name of host
  char **h_aliases;   //alias lst
  int h_addrtype;     //host address type
  int h_length;       //length of the address
  char **h_addr_list; //list of address
  };
  #define h_addr h_addr_list[0]
  
  struct sockaddr_in{ //INET socket info
  short sin_family;//set me to AF_INET
  u_short sin_port;//16 bit num, network byte order
  struct in_addr sin_addr;//32 bits host address
  char sin_zero[8];//not used
  };
  
  struct in_addr {
  union{
  struct {u_char s_b1, s_b2 , s_b3, s_b4 ; } S_un_b;
  struct {u_short s_w1,s_w2 ; } S_un_w;
  u_long S_addr ;
  }S_un;
  #define s_addr S_un.S_addr};
  
  char *inet_ntoa(const struct in_addr in);  
  int sd; 
  struct sockaddr_in_myaddr;
  int gethostname(char *name,int namelen);
  
  //en sd estoy guardando la creacion del socket
  */
/*
  
  if(sd=socket(AF_INET,SOCK_STREAM,0) < 0){
  perror("socket");//error creando el socket
  }
  myaddr.sin_family = AF_INET;
  myaddr.sin_port = htons(5100); // > 5000
  myaddr.sin_addr.s_addr= htonl(INADDR_ANY);
  //INADDR_ANY: allow OS to choose IP address for any interface
  if(bind(sd,(struct sockaddr *) &myaddr,sizeof(myaddr))<0){
  perror("bind");
  }
  
  printf ("hola mundo\n");
  
  return 0;
*/

/*
  CO SERVER
  
  listen();
    int listen(int sd,int backlog);
    
    -notify OS/newtwork ready to accept requests
    =backlog:max,listen queus size while eaiting for accept()
    -does not block/wait for requests
    
    accept();
    int accept(int sd,struct socjaddr *fromaddr,int addrlen);
    
    -use socket (sd) for accepting incoming connection requests
    - create new socket (return value) for data exchange w/client
    -block until connects , cannot selectively accpet
    
  
    connection to server: connect()
    
    format:
    
    int connect (int sd, struct socjaddr *toaddr,int addrlen);
    
    Client issues connect() to
    -establish remote address,port
    (cnnection-oriented,connectionless)
    -establish connection with server
    (connection-orinted only)
    -CO:block until accepted or error
    -fails:server/host not listening to port,timeout
  
    
    HOSTENT
    
    -address stored in h_addr is in struct in_addr form
    -to obtain "." separeted id addres we could use
    
    #include <arpa/inet.h>
    
    char(*inet_ntoa(const struct in_addr in))
    
    the routine inet_ntoa() return a pointer to a string in the base 256 notation d.d.d.d
    
    
    find out own ip address
    -find out own host name
    #Inclde <unistd.h>
    int gethostname(char *name,int namelen);
    
    -name is the characer array way to store the name of the machine with null terminated character.
    
    -namelen: size of the chracter array
    
    this function puts the name of the host in name.it returns 0 if ok, -1 if error
    
    find out own ip address (cont)
    find out own ip address
    #include <netdb.h>
    struct hostent * gethostbyname(const char *name);
    -this return hostent structure with all necessary information related to host with name.
    
    struct hostent{
    char *h_name; //canonical name of host
    char **h_aliases;//alias lst
    int h_addrtype;//host address type
    int h_length:/length of the address
    char **h_addr_list;//list of address
    };
    #define h_addr h_addr_list[0]
    
    }
  */
  
