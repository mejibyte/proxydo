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
		ClientSocket remote(headers["Host"], 80);
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

		//get the headers replied by the server
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
		}
		else if (headers["Transfer-Encoding"] == "chunked"){
			while (true){
				int chunk_size, must_read;
				string s = remote.readLine();
				sscanf(s.c_str(), "%x", &chunk_size);
				connection << s;
				must_read = 0;
				while (must_read > 0){
					int read = remote.recv(buf, min(must_read, MAXRECV));
					must_read -= read;
					connection.send(buf, read);
				}
				s = remote.readLine();
				connection << s;
				if (chunk_size == 0) break;
			}

		}else{ //read until the server closes the connection
			//while (true){
				//length = remote.recv(buf, MAXRECV);
				//connection.send(buf, length);
			//}
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