#ifndef PROXYDO_UTIL_H
#define PROXYDO_UTIL_H

#include <map>
#include <string>
#include <vector>

#define DEBUG(x) cerr << "DEBUG " << __FILE__ << ":" << __LINE__ << " " #x " = " << x << endl

namespace util {
	const char CR = '\r';
	const char LF = '\n';
	const std::string CRLF = "\r\n";
	
	//String utilities
	std::vector<std::string> split(std::string s, char delimiter = '.');
	std::string strip(std::string s);
	std::string removeTrailingLineBreaks(std::string);
	
	int toInt(std::string s);

	//HTTP related stuff
	std::map<std::string, std::string> extractHeaders(const std::string &data) throw (char *);
	std::string assembleHeaders(std::map<std::string, std::string> headers);
	std::string cleanupRequestLine(std::string);
	std::pair<std::string, int> extractHostAndPort(const std::string);
	
}

#endif
