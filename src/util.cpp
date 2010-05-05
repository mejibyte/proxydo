#include <string>
#include <map>
#include <iostream>
#include <cstdio>
#include <cctype>

#include "util.h"

using namespace std;

map<string, string> util::extractHeaders(const string &data) throw (char *){
	map<string, string> answer;
	int len = data.find(CRLF + CRLF);
	
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
