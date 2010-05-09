#ifndef INCOMING_PROXY_H
#define INCOMING_PROXY_H
#include <map>
#include <string>
	
#include "sockets/api.h"


class IncomingProxy {
	int port;
	std::map<std::string, std::string> routes;
	
    void handleConnection(ServerSocket&);
	std::pair<std::string, int> findHost(std::string &method);
public:
	IncomingProxy(int, std::map<std::string, std::string>);
	void run();
};

#endif