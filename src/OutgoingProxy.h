#ifndef OUTGOING_PROXY_H
#define OUTGOING_PROXY_H
#include <vector>
#include <string>
#include "sockets/api.h"

class OutgoingProxy {
	int port;
	//vector<string> blockedHosts;
	void DestinationThread(ServerSocket);
public:
	OutgoingProxy(int);
	void run();
};

#endif