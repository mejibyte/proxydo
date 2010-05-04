#include "sockets/api.h"
#include <string>
#include <iostream>
#include "config.h"
#include <vector>
#include "OutgoingProxy.h"
	
OutgoingProxy::OutgoingProxy(int port/*, vector<string> blockedHosts*/) : port(port) {	
}
void OutgoingProxy::run(){
	std::cout<<"[INFO] Starting..."<<std::endl;
	try{
		ServerSocket server (port);
		while ( true ){
			
			ServerSocket sock_server;
			server.accept ( sock_server );
			try{
				while ( true ){
					OutgoingProxy::DestinationThread(sock_server/*,v*/);	
				}
			}
			catch ( SocketException& ) {
			}
		}
	}
	catch ( SocketException& e ){
		std::cout << "[EXCEPTION] " << e.description() << "\n[INFO] Closing...\n";
	}
}
	
void OutgoingProxy::DestinationThread(ServerSocket sock_server/*,vector<string> blockedHosts*/){
		std::string data;
		sock_server >> data; //READ FROM ORIGIN
		//READ FROM VECTOR IF DESTINATION IS VALID
		std::cout << "[MESSAGE] " <<data<<std::endl; //MESSAGE
		//GET HOST
		//GET PORT
		try{
	      ClientSocket client_socket ( "google.com",80);
	      try{
		  client_socket << data;
		  client_socket >> data;
		}
	      catch ( SocketException& ) {}
	      std::cout << "We received this response from the server:\n\"" << data << "\"\n";;
	    }
	  catch ( SocketException& e ){
	     std::cout << "Exception was caught:" << e.description() << "\n";
	    }
		sock_server << data; //WRITE TO ORIGIN
}