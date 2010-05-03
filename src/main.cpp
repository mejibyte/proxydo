#include <iostream>
#include <cassert>
using namespace std;

#include "config.h"

int main(){
	cout << "# Starting up Proxydo" << endl;
	Config config("config.yml");
	try {
		
		int port = config.get_int("outgoing.port");
		assert(port == 8080);
		
		string s = config.get_string("incoming.default");
		assert(s == "127.0.0.1:80");
		
		bool b = config.get_bool("outgoing.enabled");
		assert(b == true);
		
		vector<string> v = config.get_vector_string("outgoing.block");
		cout << "outgoing.block:" << endl;
		for (int i = 0; i < v.size(); ++i)
			cout << v[i] << endl;
 		
		map<string, string> m = config.get_map_string_string("incoming.map");	
		cout << "incoming.map:" << endl;
		for (map<string, string>::iterator i = m.begin(); i != m.end(); ++i)
		    cout << i->first << ", " << i->second << endl;
		
	} catch (char * s){
		cout << s << endl;
	}
	return 0;
}
