#include <iostream>
#include <cassert>
#include "OutgoingProxy.h"
#include "IncomingProxy.h"
#include "config.h"
using namespace std;

int main(){
	cout << "# Starting up Proxydo" << endl;
	Config config("config.yml");
	try {
		if(config.get_bool("outgoing.enabled"))	{	
			if(fork() == 0){
				OutgoingProxy op(config.get_int("outgoing.port"), config.get_vector_string("outgoing.block"));
				op.run();
				exit(0);
			}
		}
		if(config.get_bool("incoming.enabled")) {
			if(fork() == 0){
				int Iport  = 10001;
				IncomingProxy ip(Iport);
				ip.run();
				exit(0);
			}

		}
	} catch (char * s){
		cout << s << endl;
	}
	return 0;
}
