#ifndef PROXYDO_CONFIG_H
#define PROXYDO_CONFIG_H

#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <yaml.h>
#include <iostream>
class Config {
	std::auto_ptr<YAML::Node> get_node(std::string property) throw (char *);
	
public:
	std::string filename;

	Config(std::string);
	
	bool get_bool(std::string);
	int get_int(std::string);
	std::string get_string(std::string);
	std::vector<std::string> get_vector_string(std::string);
	std::map<std::string, std::string> get_map_string_string(std::string);
	
	std::vector<std::string> split(std::string, char = '.');
};

#endif