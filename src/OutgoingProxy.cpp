#include <iostream>

#include "sockets/api.h"

#include "OutgoingProxy.h"
#include "config.h"
#include "util.h"

using namespace std;

OutgoingProxy::OutgoingProxy(int port, vector<string> blockedHosts) : port(port), blockedHosts(blockedHosts) {
	cout << "# [Outgoing] Initializing outgoing proxy:" << endl;	
	cout << "# [Outgoing]      Port: " << port << endl;
	cout << "# [Outgoing]      Blocked hosts:" << endl;
	for (int i = 0; i < blockedHosts.size(); ++i){
		cout << "# [Outgoing]          " << blockedHosts[i] << endl;
	}
}
void OutgoingProxy::run(){
	try {
		ServerSocket socket(port);
		cout << "# [Outgoing] Binding socket on port " << port << endl;
		while (true){			
			ServerSocket connection;
			cout << "# [Outgoing] Accepting connections on port " << port << "..." << endl;
			socket.accept ( connection );
			cout << "# [Outgoing] Connection received" << endl;
			OutgoingProxy::DestinationThread(connection);	
		}
	}
	catch ( SocketException& e ){
		cout << "# [Outgoing] Exception: " << e.description() << endl;		
	}
}

void OutgoingProxy::DestinationThread(ServerSocket &connection){
	try {

		string header = "";
		while (true){
			string s = connection.readLine();
			if (s.find("Accept-Encoding") == 0) continue;
			header += s;
			if (s == "\n" or s == "\r\n") break;
		}

		map<string, string> headers = util::extractHeaders(header);
		//extract host

		ClientSocket remote("74.125.67.103", 80);
		remote << header;
		
		cout << header;

		char buf[MAXRECV];
		int length;

		//Send the body of the request
		int must_send = 0;
		if (headers.count("Content-Length")) must_send = util::toInt(headers["Content-Length"]);
		while (must_send > 0){
			length = connection.recv(buf, MAXRECV);
			must_send -= length;
			remote.send(buf, length);
		}

		//get the headers replied by the server the server
		header = "";
		while (true){
			string s = remote.readLine();
			header += s;
			if (s == "\n" or s == "\r\n") break;
		}

		cout << header;
		connection << header;

		headers = util::extractHeaders(header);
		//Send the body replied by the server.
		if (headers.count("Content-Length")){
			must_send = util::toInt(headers["Content-Length"]);
			while (must_send > 0){
				length = remote.recv(buf, MAXRECV);
				must_send -= length;
				connection.send(buf, length);
			}
		}else if (headers["Transfer-Encoding"] == "chunked"){
			while (true){
				string s = remote.readLine();
				connection << s;
				if (s == "\n" or s == "\r\n") break;
			}
		}


		remote.~ClientSocket();
		connection.~ServerSocket();
	}
	catch (SocketException& e){
		cout << "# [Outgoing] Exception: " << e.description() << endl;				
	}
	catch (char * s){
		cout << "# [Outgoing] Exception: " << s << endl;						
	}
}