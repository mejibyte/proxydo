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
			if (!socket.is_valid()) socket = ServerSocket(port);
			ServerSocket * connection = new ServerSocket();
			cout << "# [Outgoing] Accepting connections on port " << port << "..." << endl;
			socket.accept ( *connection );
			cout << "# [Outgoing] Accepted connection" << endl;
			OutgoingProxy::createThread(*connection);
		}
	}
	catch ( SocketException& e ){
		cout << "# [Outgoing] Exception: " << e.description() << endl;		
	}
}

struct ThreadParameters{
	OutgoingProxy proxy;
	ServerSocket connection;
};

void * runThread(void * arg){
	try {
		ThreadParameters * params = (ThreadParameters *)arg;
		cout << "# [Outgoing] Hello. I'm a thread and I must handle a new connection" << endl;
		params->proxy.handleConnection(params->connection);
    	delete params;		
	} catch (SocketException &e){
		cout << "# [Outgoing] Exception in thread: " << e.description() << endl;
	}
	cout << "# [Outgoing] Terminating thread" << endl;
	pthread_exit(NULL);
}

void OutgoingProxy::createThread(ServerSocket &connection){
	cout << "# [Outgoing] Creating thread to handle new connection" << endl;
	pthread_t thread;
	ThreadParameters * p = new ThreadParameters();
	p->proxy = OutgoingProxy(*this);
	p->connection = connection;
	pthread_create(&thread, NULL, runThread, (void *)p);
}


void OutgoingProxy::handleConnection(ServerSocket &connection){
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
				connection.send(s.c_str(), s.size());

				must_read = chunk_size;
				while (must_read > 0){
					int read = remote.recv(buf, min(must_read, MAXRECV));
					must_read -= read;
					connection.send(buf, read);
				}
				s = remote.readLine();
				connection << s;
				if (chunk_size == 0) break;
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