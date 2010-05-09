#include <iostream>
#include <cassert>
#include <vector>
#include <csignal>
#include <cstdlib>
#include <sys/wait.h>

#include "OutgoingProxy.h"
#include "IncomingProxy.h"
#include "config.h"
using namespace std;


vector<int> forks;

void sig_int(int signal){
	if (signal == SIGINT){
		cout << "# Received SIGINT signal. Killing forks:" << endl;
		for (int i = 0; i < forks.size(); ++i){
			kill(forks[i], SIGKILL);
			cout << "#    Killed fork with pid " << forks[i] << endl;
		}
		exit(0);		
	}
}

void sig_chld(int signal){
	if (signal == SIGCHLD){
		while (waitpid(-1, NULL, WNOHANG) > 0);
	}
}

int main(){
	cout << "# Starting up Proxydo" << endl;
	signal(SIGINT, sig_int);
	signal(SIGCHLD, sig_chld);

	Config config("config.yml");
	try {
		if(config.get_bool("outgoing.enabled"))	{	
			int p = fork();
			if(p == 0){
				OutgoingProxy op(config.get_int("outgoing.port"), config.get_vector_string("outgoing.block"));
				op.run();
				exit(0);
			}else if (p > 0){
				forks.push_back(p);
			}
		}
		if(config.get_bool("incoming.enabled")){
			int p = fork();
			if(p == 0){
				IncomingProxy ip(config.get_int("incoming.port"), config.get_map_string_string("incoming.routes"));
				ip.run();
				exit(0);
			}else if (p > 0){
				forks.push_back(p);
			}

		}
	} catch (char * s){
		cout << s << endl;
	}

	while (true) {
		usleep(100);
	}
	return 0;
}
