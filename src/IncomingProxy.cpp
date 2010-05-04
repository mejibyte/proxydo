#include <iostream>
#include <pthread.h>
#include "../sockets_api/ClientSocket.h"
#include "../sockets_api/ServerSocket.h"
#include "IncomingProxy.h"
#define NUM_THREADS 100

IncomingProxy::IncomingProxy(int port) : port (port) {
}

void IncomingProxy::run(){
    try {
      ServerSocket proxy (port);
        while (true) {
          ServerSocket connection;
          proxy.accept(connection);

          pthread_t threads[NUM_THREADS];
          int request_thread;
          long t;
          for(t=0; t<NUM_THREADS; t++){
            request_thread = pthread_create(&threads[t], NULL, getResource, (void *)connection);
          }
        }
          pthread_exit(NULL);
        }
      }
    catch (SocketException& e)  {
      //Exception catched
    }
  }  
