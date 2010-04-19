#include <iostream>
#include "servercppMethod.cpp"
using namespace std;
void escuhador(argumentos *argos){
  struct argumentos *a = argos;
  int t;
  t = a->t;
  ClientServer Client(t);
  Client.ClientReadSource();
  Client.ClientConnect();
  Client.ClientWriteDestiny();
  while(Client.ClientReadDestiny()){
     if(!Client.ClientWriteSource()){
		break;
     }
  }
  Client.ClientCloseSocketSource();
  Client.ClientCloseSocketDestiny();
  printf("\n========================\n");
}
void *ThreadMain(void *clntSock) {
  pthread_detach(pthread_self()); 
  escuhador((argumentos *) clntSock);
  pthread_exit((void *)clntSock);
  return NULL;
}

int main(int argc, char *argv[]){
  socketServer socketA(atoi(argv[1]));
  socketServer socketB(atoi(argv[2]));
  int t = 0;
  int error,error1,t1 = 0;
  switch(fork()){
  case -1:
    cout<<"[ERRO] proceso"<<endl;
    break;
  case 0:
    socketB.ServerListen(5);
    while(true){
      pthread_t threadB;
      cout<<"[INFO] Servidor escucha puerto: "<<atoi(argv[2])<<endl;
      t1 = socketB.ServerSockAccept();
      argumentos arguB;
      arguB.t = t1;
      if(error1 = pthread_create(&threadB,NULL,ThreadMain,(void *) &arguB)==-1){
			perror((char *)error1);		
 			break;
	}
	//sleep(5);   
}
    break;
  default:
    socketA.ServerListen(5);
    while(true){
      pthread_t threadA;
      cout<<"[INFO] Servidor escucha puerto: "<<atoi(argv[1])<<endl;
      t = socketA.ServerSockAccept();
      argumentos arguA;
      arguA.t = t;
      if(error = pthread_create(&threadA,NULL,ThreadMain,(void *) &arguA)==-1){
	    perror((char *)error);	
		break;
		}
    }
	//sleep(5);
    break;		
  }
  socketA.CloseServerSock();
  socketB.CloseServerSock();
  return 0;
}
