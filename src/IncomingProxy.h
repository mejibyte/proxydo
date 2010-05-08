#ifndef INCOMING_PROXY_H
#define INCOMING_PROXY_H
#include "sockets/api.h"
class IncomingProxy {
	int port;
        void handleConnection(ServerSocket&);
public:
	IncomingProxy(int);
	void run();
};

#endif