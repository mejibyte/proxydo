/*stream client:send a message to server*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

struct hostent *gethostbyname(const char *name);

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

int main(int argc, char *argv[]){
  
  int s,n;
  struct sockaddr_in sin;
  struct hostent *hptr;
  char msg[800]= "";
  
  while(strcmp("stop",&msg)){
    printf("[MENSAJE] :");
    gets(&msg);
      printf("[INFO] Mensaje a enviar: %s\n",msg);
    
    if(argc <3){
      printf("%s host port\n",argv[0]);//input error:need host and port
      return -1;
    }
    
    
    if((s=socket(AF_INET,SOCK_STREAM,0))<0){//create socket
      perror("socket");//socket error
      return -1;
    }
    
    sin.sin_family = AF_INET;//set portocol family to internet
    sin.sin_port = htons(atoi(argv[2]));//set port number
    
    //puerto q se le asigna al sin_port    printf("port %d\n",sin.sin_port);
    
    if ((hptr = gethostbyname(argv[1]))== NULL){
      fprintf(stderr,"[ERROR] gethostname error: %s",argv[1]);
      return -1;
    }
    
   // memcpy(&sin.sin_addr,hptr->h_addr,hptr->h_length);
    
    printf("[INFO] Conectando...\n") ; 
    
    if (connect(s,(struct sockaddr *)&sin,sizeof(sin))<0){
      perror("connect");
      return -1; //error while connect
    }
    printf("[INFO] Conectado con exito...\n");
    
    printf("[INFO] Enviando mensaje al servidor...\n");
    
	
    if(write(s,msg,strlen(msg)+1)<0){ //send messeage to server
      perror("write");
      return -1;//error while write
    }
    printf("[INFO] Mensaje Enviado...\n");
    
    printf("[INFO] Leer mensaje desde servidor...\n");
    
    if((n = read(s,msg,sizeof(msg)))<0){//read message form server
      perror("read");
    return -1;//error read
    }
    printf("[INFO] Mensaje Leido..\n");  
    
    printf("[INFO] Escribiendo Mensaje en Pantalla..\n");
    printf("[MENSAJE] %d bytes: %s\n",n,msg);//print message to screen
    //close connection , clean up socket
    
    printf("[CERRANDO CONEXION]\n");
    if(close(s)<0){
      perror("close");//close error
      return -1;
    }
    printf("[FIN].\n");
    printf("\n========================\n");
  }
  return 0;
}
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
    int accept(int sd,struct sockaddr *fromaddr,int addrlen);
    
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
  
