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
		string data;
		connection >> data;

		cout << "# [Outgoing] Received request from client:" << endl;
		cout << data << endl;
		cout << "# [Outgoing] About to redirect the request to the server" << endl;	

		map<string, string> headers = util::extractHeaders(data);
		//extract host
		
		ClientSocket remote("www.google.com.co", 80);
		remote << data;
		
		string reply;
		remote >> reply;
		
		cout << reply << endl;
		
		connection << reply;

		remote.~ClientSocket();
		connection.~ServerSocket();
	}
	catch (SocketException& e){
		cout << "# [Outgoing] Exception: " << e.description() << endl;				
	}
}