#include <string>
#include <map>
#include <iterator>
#include <iostream>
#include <cstdio>
#include <cctype>
#include <sstream>

#include "util.h"

using namespace std;

map<string, string> util::extractHeaders(const string &data) throw (char *){
	map<string, string> answer;
	int len = data.find(CRLF + CRLF);
	if (len == string::npos) len = data.find(LF + LF);	
	if (len == string::npos){
		throw "Invalid data. Cannot find end of headers.";
	}
	vector<string> headers = split(data.substr(0, len), '\n');
	for (int i = 0; i < headers.size(); ++i){
		const string &h = headers[i];
		int colon = h.find(":");
		int space = h.find(" ");
		if (space == string::npos) space = h.size();
		if (colon != string::npos and colon < space){
			answer[strip(h.substr(0, colon))] = strip(h.substr(colon + 1, h.length()));
		}
	}
	return answer;
}

string util::assembleHeaders(map<string, string> headers){
	string answer = "";
	for (map<string, string>::iterator i = headers.begin(); i != headers.end(); ++i){
		answer += i->first + ": " + i->second + "\r\n";
	}
	return answer;
}


//Receives the first line of an HTTP header
//and makes sure the host starts with a slash.
//Example:
//  converts "GET http://www.google.com/something/nice HTTP/1.1"
//  into     "GET /something/nice HTTP/1.1"
string util::cleanupRequestLine(string s){
	vector<string> parts = split(s, ' ');
	if (parts.size() != 3){
		throw "Invalid request line. It should contain exactly 3 parts separated by spaces. Example: 'GET / HTTP/1.1";
	}
	string resource = parts[1];	
	if (resource.find("/") != 0){
		//does not start with a slash
		if (resource.find("http://") == 0){
			int nextSlash = resource.find("/", string("http://").size());
			if (nextSlash == string::npos){
				throw "Invalid request line. The URI should contain at least one slash after http://";
			}
			resource = resource.substr(nextSlash, resource.size());
		} else {
			int nextSlash = resource.find("/");
			if (nextSlash == string::npos){
				throw "Invalid request line. The URI should contain at least one slash";
			}			
			resource = resource.substr(nextSlash, resource.size());
		}
	}
	return parts[0] + " " + resource + " " + parts[2];
}

// Returns s without trailing and leading withespace
string util::strip(string s) {
	int i = 0, j = s.size() - 1;
	while (i < s.size() && isspace(s[i])) i++;
	while (j >= 0 && isspace(s[j])) j--;
	if (j <= i) return "";
	return s.substr(i, j - i + 1);
}


// Splits s by delimiter, and returns a vector with all the parts.
vector<string> util::split(string s, char delimiter) {
	s += delimiter;
	vector<string> ans;
	string t = "";
	for (int i = 0; i < s.size(); ++i){
		if (s[i] == delimiter){
			ans.push_back(t);
			t = "";
		}else{
			t += s[i];
		}
	}
	return ans;
}


int util::toInt(string s){
	stringstream sin(s);
	int x;
	sin >> x;
	return x;
}