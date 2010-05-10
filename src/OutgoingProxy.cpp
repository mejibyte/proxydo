#include <iostream>
#include <set>
#include <cstdlib>

#include "sockets/api.h"

#include "OutgoingProxy.h"
#include "config.h"
#include "util.h"
#include "base64.h"

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

void OutgoingProxy::run(){
	try {
		ServerSocket proxy(port);
		cout << "# [Outgoing] Binding socket on port " << port << endl;
		while (true){	
			ServerSocket connection;
			cout << "# [Outgoing] Accepting connections on port " << port << "..." << endl;
			proxy.accept ( connection );
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
  Config config("config.yml");
  for (int i = 0; i < blockedHosts.size(); ++i){
    if (host.find(blockedHosts[i]) != string::npos){
      string reply = "<html><body>Blocked by Proxydo</body></html>";			
      connection << "HTTP/1.1 200 OK\r\n\r\n" << reply;
      if (config.get_bool("outgoing.report")) OutgoingProxy::reportBlockedHost(host);
      return true;
    }
  }
  return false;
}

void OutgoingProxy::reportBlockedHost(string host) {
  Config config("config.yml");
  ClientSocket mailserver(config.get_string("outgoing.smtpserver"), config.get_int("outgoing.smtpport"));
  string hostname = "HELO "+config.get_string("outgoing.hostname")+"\r\n";
  string to = "RCPT TO: "+config.get_string("outgoing.to")+"\r\n";
  string message = config.get_string("outgoing.message")+"\r\n\r\n"+host+"\r\n";
  // Username and password are encoded in BASE64.
  const std::string u = config.get_string("outgoing.username");
  const std::string p = config.get_string("outgoing.password");
  string user = base64_encode(reinterpret_cast<const unsigned char*>(u.c_str()), u.length())+"\r\n";
  string passwd = base64_encode(reinterpret_cast<const unsigned char*>(p.c_str()), p.length())+"\r\n";
  string from = "MAIL FROM: "+config.get_string("outgoing.from")+"\r\n";
  string subject = "Subject: "+config.get_string("outgoing.subject")+"\r\n";
  
  // Sending mail
  mailserver << hostname;
  sleep(0.2);
  mailserver << "AUTH LOGIN\r\n";
  sleep(0.2);
  mailserver << user;
  sleep(0.2);
  mailserver << passwd;
  sleep(0.2);
  mailserver << from;
  sleep(0.2);
  mailserver << to;
  sleep(0.2);
  mailserver << "DATA\r\n";
  mailserver << subject;
  mailserver << message;
  mailserver << "\r\n.\r\nQUIT\r\n";

  mailserver.~ClientSocket();
  cout << "Mail sent to: " << config.get_string("outgoing.to") << endl;
}

void OutgoingProxy::handleConnection(ServerSocket &connection){
	try {
		string requestLine = connection.readLine(); //GET / HTTP/1.1
		requestLine = util::cleanupRequestLine(requestLine);		
		string header = requestLine + "Connection: close\r\n";
		while (true){
			string s = connection.readLine();
			if (s.find("Connection") == 0) continue;
			header += s;
			if (s == "\n" or s == "\r\n") break;
		}

		map<string, string> headers = util::extractHeaders(header);

		if (checkBlockedHost(headers["Host"], connection)) throw SocketException("Blocked host");

		ClientSocket webserver(headers["Host"], 80);
		webserver << header;
		cout << header;

		Socket::relay_connection(connection, webserver);
		
		webserver.~ClientSocket();
		connection.~ServerSocket();
	}
	catch (SocketException& e){
		cout << "# [Outgoing] Exception: " << e.description() << endl;				
	}
	catch (char * s){
		cout << "# [Outgoing] Exception: " << s << endl;						
	}
}


