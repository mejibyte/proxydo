#include <iostream>
#include <set>
#include <cstdlib>
#include <string>
#include <map>

#include "sockets/api.h"

#include "IncomingProxy.h"
#include "config.h"
#include "util.h"

using namespace std;

IncomingProxy::IncomingProxy(int port, map<string, string> routes) : port (port), routes(routes) {
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

pair<string, int> IncomingProxy::findHost(string &requestLine){
	vector<string> parts = util::split(requestLine, ' ');
	if (parts.size() != 3){
		throw "Request line should have exactly 3 parts separated by spaces. Example: GET /search HTTP/1.1";
	}
	string requestUri = parts[1];
	if (requestUri.find("/") != 0){
		throw "The second part of the request line should start with a slash. Example: GET /search HTTP/1.1";
	}
	
	pair<string, int> answer("127.0.0.1", 80);
	
	for (map<string, string>::iterator i = routes.begin(); i != routes.end(); ++i){
		const string &uri = i->first;
		const string &host = i->second;

		requestUri += "/";
		if (requestUri.find("/" + uri + "/") == 0){
		//Found a match! The second part starts with uri.
		//Example:
		//   requestLine = GET /site1/search HTTP/1.1
		//   uri         = site1
			answer = util::extractHostAndPort(host);
			requestUri.resize(requestUri.size() - 1); //Delete the trailing slash we added before			
			requestUri.erase(0, 1 + uri.size());
			if (requestUri.size() == 0) requestUri = "/";
			break; //Found a match, stop looking.
		}else{
			requestUri.resize(requestUri.size() - 1); //Delete the trailing slash we added before			
		}

	}
	requestLine = parts[0] + " " + requestUri + " " + parts[2];

	return answer;
}

void IncomingProxy::handleConnection(ServerSocket &connection) {
  try {
    string requestLine = connection.readLine();
	requestLine = util::cleanupRequestLine(requestLine);
    string header = requestLine + "Connection: close\r\n";

	//Read header
    while (true) {
      string s = connection.readLine();
      if (s.find("Connection") == 0) continue;
      header += s;
      if (s == "\n" or s == "\r\n") break;
    }

	map<string, string> headers = util::extractHeaders(header);
	
	//Be careful, the requestLine is modified when calling findHost.
	//That is the intended behaviour.
	pair<string, int> newHost = findHost(requestLine);
	
	cout << "# [Incoming] '" << util::removeTrailingLineBreaks(requestLine)
	 						 << "' at " << newHost.first << ":" << newHost.second << endl;
	
	headers["Host"] = newHost.first;
	header = requestLine + util::assembleHeaders(headers);	

    ClientSocket webserver(newHost.first, newHost.second);
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
