#include <iostream>
#include <set>

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

OutgoingProxy::OutgoingProxy(const OutgoingProxy &other){
	port = other.port;
	blockedHosts = other.blockedHosts;
}

OutgoingProxy::OutgoingProxy() : port(8080) {
}

void OutgoingProxy::run(){
	try {
		ServerSocket socket(port);
		cout << "# [Outgoing] Binding socket on port " << port << endl;
		while (true){	
			ServerSocket connection;
			cout << "# [Outgoing] Accepting connections on port " << port << "..." << endl;
			socket.accept ( connection );
			cout << "# [Outgoing] Accepted connection" << endl;
			if (fork() == 0){
				OutgoingProxy::handleConnection(connection);
				exit(0);				
			}
		}
	}
	catch ( SocketException& e ){
		cout << "# [Outgoing] Exception: " << e.description() << endl;		
	}
}

bool OutgoingProxy::checkBlockedHost(string host, ServerSocket &connection){
	for (int i = 0; i < blockedHosts.size(); ++i){
		if (host.find(blockedHosts[i]) != string::npos){
			string reply = "<html><body>Blocked by Proxydo</body></html>";			
			connection << "HTTP/1.1 200 OK\r\n\r\n" << reply; 			
			return true;
		}
	}
	return false;
}

void OutgoingProxy::handleConnection(ServerSocket &connection){
	try {
		string header = connection.readLine(); //GET / HTTP/1.1
		header += "Connection: close\r\n";
		while (true){
			string s = connection.readLine();
			if (s.find("Connection") == 0) continue;
			header += s;
			if (s == "\n" or s == "\r\n") break;
		}

		map<string, string> headers = util::extractHeaders(header);

		if (checkBlockedHost(headers["Host"], connection)) throw SocketException("Blocked host");

		ClientSocket remote(headers["Host"], 80);
		remote << header;
		cout << header;

		Socket::relay_connection(connection, remote);
		
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