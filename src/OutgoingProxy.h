#ifndef OUTGOING_PROXY_H
#define OUTGOING_PROXY_H
#include <vector>
#include <string>
#include "sockets/api.h"

class OutgoingProxy {
	int port;
	std::vector<std::string> blockedHosts;
	void DestinationThread(ServerSocket&);
public:
	OutgoingProxy(int,  std::vector<std::string>);
	void run();
};

#endif