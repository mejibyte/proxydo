#ifndef INCOMING_PROXY_H
#define INCOMING_PROXY_H

class IncomingProxy {
	int port;

public:
	IncomingProxy(int);
	void run();
};

#endif