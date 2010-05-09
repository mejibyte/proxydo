#ifndef PROXYDO_UTIL_H
#define PROXYDO_UTIL_H

#include <map>
#include <string>
#include <vector>

namespace util {
	const char CR = '\r';
	const char LF = '\n';
	const std::string CRLF = "\r\n";
	
	std::vector<std::string> split(std::string s, char delimiter = '.');
	std::string strip(std::string s);
	
	int toInt(std::string s);

	std::map<std::string, std::string> extractHeaders(const std::string &data) throw (char *);
	std::string assembleHeaders(std::map<std::string, std::string> headers);
	
}

#endif
