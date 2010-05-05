#ifndef PROXYDO_UTIL_H
#define PROXYDO_UTIL_H

#include <map>
#include <string>
#include <vector>

namespace util {
	const char CR = 13;
	const char LF = 10;
	const std::string CRLF = "\x0D\x0A";
	
	std::vector<std::string> split(std::string s, char delimiter = '.');
	std::string strip(std::string s);

	std::map<std::string, std::string> extractHeaders(const std::string &data) throw (char *);
	
}

#endif
