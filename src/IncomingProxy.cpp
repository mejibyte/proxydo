#include <iostream>
#include <set>
#include <cstdlib>

#include "sockets/api.h"

#include "IncomingProxy.h"
#include "config.h"
#include "util.h"

using namespace std;

IncomingProxy::IncomingProxy(int port) : port (port) {
	cout << "# [Incoming] Initializing incoming proxy:" << endl;	
	cout << "# [Incoming]      Port: " << port << endl;
}

void IncomingProxy::run(){
  try {
    ServerSocket proxy(port);
    cout << "# [Incoming] Binding socket on port " << port << endl;
    while (true) {
      ServerSocket connection;
      cout << "# [Incoming] Accepting connections on port " << port << "..." << endl;
      proxy.accept(connection);
      cout << "# [Incoming] Accepted connection" << endl;

      if (fork() == 0){
        IncomingProxy::handleConnection(connection);
        exit(0);
      }
    }
  }
  catch (SocketException& e)  {
    cout << "# [Incoming] Exception: " << e.description() << endl;		
  }
}

void IncomingProxy::handleConnection(ServerSocket &connection) {
  try {
    string header = connection.readLine();
    header += "Connection: close\r\n";
    while (true) {
      string s = connection.readLine();
      if (s.find("Connection") == 0) continue;
      header += s;
      if (s == "\n" or s == "\r\n") break;
    }

    //map<string, string> headers = util::extractHeaders(header);

    //ClientSocket webserver(headers["Host"], 80);
    ClientSocket webserver("www.google.com", 80);
    webserver << header;
    cout << header;
    
    Socket::relay_connection(connection, webserver);

    webserver.~ClientSocket();
    connection.~ServerSocket();

  }
  catch (SocketException& e)  {
    cout << "# [Incoming] Exception: " << e.description() << endl;
  }
  catch (char * s){
    cout << "# [Incoming] Exception: " << s << endl;						
  }
}
