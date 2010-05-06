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

      proxy.~ServerSocket();
      processConnection(connection);
    }
  }
  catch (SocketException& e)  {
    cout << "Proxy: " << e.description() << endl;
  }
}

void IncomingProxy::processConnection(ServerSocket connection) {
  string httpRequest;
  string httpResponse;
  string trash;
  while (true) {
  try {
    // Connect to web server
    // Reading REQUEST from Browser
    // Saves request from client in httpRequest
    connection >> httpRequest;
    cout << httpRequest << endl;
    // Sending request to Webserver
    //ws << httpRequest;
    ClientSocket ws ("localhost", 80);
    ws << httpRequest;
    usleep(20000);
    // Saving server response in httpResponse
    //int total = (ws + httpResponse); //Getting Response
    //cout << "Total bytes: " << total << endl;
    //connection << httpResponse; //Sending response to the user
    //cout << httpResponse << endl;
    int total = 1;
    while(total>0)
      {
        total = (ws + httpResponse);
        cout << "Total in " << total << endl;
        if(total > 0)
          {
            cout << httpResponse << endl;
            while (true)
              connection << httpResponse;
          }
        else
          {
            break;// error reading from socket
          }
        }
    connection.~ServerSocket();
    ws.~ClientSocket();
      //exit(0);
  }
  catch (SocketException& e)  {
    cout << "Client: " << e.description() << endl;
  }
  }
}
