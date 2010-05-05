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
  while (true) {
    }
      try {
        // Connect to web server
        ClientSocket cs ("localhost", 80);
        while (true) {
          // Saves request from client in httpRequest
          connection >> httpRequest;
          cout << httpRequest << endl;
          // Redirect request to Webserver
          cs << httpRequest;
          // Saves response from Webserver in httpResponse
          cs >> httpResponse;
          // Returns servers response to browser
          connection << httpResponse;
          cout << httpResponse << endl;
        }
      }
      catch (SocketException& e)  {
        cout << "Client: " << e.description() << endl;
      }
}
