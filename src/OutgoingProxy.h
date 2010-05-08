#ifndef OUTGOING_PROXY_H
#define OUTGOING_PROXY_H
#include <vector>
#include <string>
#include <map>
#include "sockets/api.h"

class OutgoingProxy {
	std::vector<std::string> blockedHosts;
	void createThread(ServerSocket&);
	bool checkBlockedHost(std::string host, ServerSocket &connection);
public:
	int port;	
	OutgoingProxy(int,  std::vector<std::string>);
	OutgoingProxy(const OutgoingProxy &other);
	OutgoingProxy();
	void handleConnection(ServerSocket&);		
	void run();
};

#endif