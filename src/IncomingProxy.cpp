#include <iostream>
#include <pthread.h>
#include <string.h>
#include "sockets/api.h"
#include "sockets/ServerSocket.h"
#include "IncomingProxy.h"

#define NUM_THREADS 100

IncomingProxy::IncomingProxy(int port) : port (port) {
}

using namespace std;

void IncomingProxy::run(){
  try {
    // Port where proxy runs
    ServerSocket proxy(port);
    while (true) {
      // Connection with the client
      ServerSocket connection;
      proxy.accept(connection);

      while (true) {
      processConnection(connection);
      }
    }
  }
  catch (SocketException& e)  {
    cout << "Proxy: " << e.description() << endl;
  }
}

void IncomingProxy::processConnection(ServerSocket connection) {
  string httpRequest;
  string httpResponse;
  try {
    // Connect to web server
    ClientSocket ws ("localhost", 80);
    //while (true) {
      // Saves request from client in httpRequest
      connection >> httpRequest;
      cout << httpRequest << endl;
      // Redirect request to Webserver
      ws << httpRequest;
      //usleep(200000);
      int total = (ws + httpResponse);//Getting Response
      connection << httpResponse;//Sending response to the user
      cout << httpResponse << endl;
      while(total>0)
        {
          //usleep(5000);
          total = (ws + httpResponse);
          if(total > 0)
            {
              cout << httpResponse << endl;
              connection << httpResponse;
              //  cerr<<data;
            }
          else
            {
              break;// error reading from socket
            }
        }
      // Saves response from Webserver in httpResponse
      //ws >> httpResponse;
      // Returns servers response to browser
      //connection << httpResponse;

      //}
  }
  catch (SocketException& e)  {
    cout << "Client: " << e.description() << endl;
  }
  
}
