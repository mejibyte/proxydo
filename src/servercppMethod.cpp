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
#include <iostream>
#include <stdio.h>

class socketServer{
public:
  int s,t,sinlen;
  struct sockaddr_in sin;
  struct hostent *hptr;
  socketServer(int port){
    if((s=socket(AF_INET,SOCK_STREAM,0))<0){//create socket
      perror("socket");//socket error
      exit(1);
    }
    sin.sin_family = AF_INET;//set portocol family to internet
    sin.sin_port = htons(port);//set port number
    sin.sin_addr.s_addr = INADDR_ANY;//set ip address to any interface
    if(bind(s,(struct sockaddr *)&sin,sizeof(sin))<0){
      perror("bind");//bind error
      exit(1);
    }
  }
  int ServerSockAccept(){
    int t;
    if((t=accept(s,( sockaddr  *) &sin,( socklen_t *)&sinlen))<0){
      puts("[ERROR] fork con socket");
      perror("accept"); 
      exit(1);
    }
	printf("[IP ENTRANTE] %s [PUERTO ENTRANTE] %d\n",inet_ntoa(sin.sin_addr),ntohs(sin.sin_port));
    return t;
  }
  void ServerListen(int n){
    if(listen(s,n)){
      puts("[ERROR]");
      perror("listen");
      exit(1);
    }
  }
  void CloseServerSock(){
	puts("[CERRANDO CONEXION SERVIDOR]");
    if(close(s)<0){
      perror("close");
      exit(1);
    }
  }
};

class ClientServer{
public:
  char msg[4000];
  int s,t,sinlen;
  struct sockaddr_in sin;
  struct hostent *hptr;
  ClientServer(int socketid){
    t = socketid;
    puts("[INFO] Creando socket Client/Servidor...");
    if((s=socket(AF_INET, SOCK_STREAM, 0))<0){//create socket
      puts("[ERROR] create s");
      perror("socket");//socket error
      exit(1);
    }
    sin.sin_family = AF_INET;//set portocol family to internet
    sin.sin_port = htons(80);//set port number
    puts("[INFO] Verificando direccion destino...");	
    if ((hptr = gethostbyname("localhost"))== NULL){
      fprintf(stderr,"[ERROR] gethostname error: %s\n","localhost");
      exit(1);
    }
    memcpy(&sin.sin_addr,hptr->h_addr,hptr->h_length);
  }
  bool ClientWriteSource(){
	puts("[INFO] Enviando mensaje a la fuente...");
    if(write(t,msg,strlen(msg))<0){ //send messeage to server
      puts("[ERROR] write s");
      perror("write");
	  //exit(1);
	  return false;
    }
    puts("[INFO] Mensaje Enviado...");
	return true;
	}
  void ClientWriteDestiny(){
	  puts("[INFO] Enviando mensaje al servidor...");
    if(write(s,msg,strlen(msg))<0){ //send messeage to server
      puts("[ERROR] write s");
      perror("write");
      exit(1);
    }
    puts("[INFO] Mensaje Enviado...");
  }
  void ClientConnect(){
	puts("[INFO] Conectando con Servidor...") ; 
    if(connect(s,(struct sockaddr *)&sin,sizeof(sin))<0){
      puts("[ERROR] connect s");
      perror("connect");
      exit(1);
    }
    puts("[INFO] Conectado con exito...");
  }
  bool ClientReadDestiny(){
	puts("[INFO] Leyendo Mensaje del Servidor...");
    if(read(s,msg,sizeof(msg))<0){//Read message from client
      puts("[ERROR] read first s");
      perror("read");
	  //exit(1); 
	return false;
    }
	puts("[INFO] Mensaje Leido de Servidor..");
	printf("[MENSAJE] %s\n",msg);
	return true;
  }
  
  void ClientReadSource(){
	puts("[INFO] Leyendo Mensaje de fuente...");
    if(read(t,msg,sizeof(msg))<0){//Read message from client
      puts("[ERROR] read first t");
      perror("read");
      exit(1);
    }
	puts("[INFO] Mensaje Leido de fuente..");
	printf("[MENSAJE] %s\n",msg);
  }
  void ClientCloseSocketSource(){
	puts("[CERRANDO CONEXION FUENTE]");
    if(close(t)<0){
      puts("[ERROR] cerrando t ");
      perror("close");//close error
      exit(1);
    }
  }
  void ClientCloseSocketDestiny(){
	puts("[CERRANDO CONEXION SERVIDOR]");
    if(close(s)<0){
      puts("[ERROR] cerrando s ");
      perror("close");//close error
      exit(1);
    }
  }
};

struct hostent *gethostbyname(const char *name);
struct argumentos{
  int t;
};