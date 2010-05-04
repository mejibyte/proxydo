#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <yaml.h>
using namespace std;

#include "config.h"

Config::Config(string filename) : filename(filename) {
	cout << "# Initializing configuration file: " << filename << endl;
}

std::auto_ptr<YAML::Node> Config::get_node(string property) throw (char *){
	std::ifstream fin(filename.c_str());
	if (!fin){
		throw "Can not open configuration file";
	}
	YAML::Parser parser(fin);
	YAML::Node doc;
	if (!parser.GetNextDocument(doc)){
		throw "Can not parse configuration file";
	}
	vector<string> path = split(property);
	std::auto_ptr<YAML::Node> node = doc.Clone();

	for (int i = 0; i < path.size(); ++i){
		node = (*node)[path[i]].Clone();
	}
	fin.close();
	return node;	
}


bool Config::get_bool(string property){
	std::auto_ptr<YAML::Node> node = get_node(property);
	bool ans;
	(*node) >> ans;
	return ans;
}


int Config::get_int(string property){
	std::auto_ptr<YAML::Node> node = get_node(property);
	int ans;
	(*node) >> ans;
	return ans;
}

string Config::get_string(string property) {
	std::auto_ptr<YAML::Node> node = get_node(property);
	string ans;
	(*node) >> ans;
	return ans;
}

vector<string> Config::get_vector_string(string property){
	std::auto_ptr<YAML::Node> node = get_node(property);
	vector<string> ans;
	for (int i = 0; i < node->size(); ++i){
		ans.push_back((*node)[i]);
	}
	return ans;	
}

map<string, string> Config::get_map_string_string(string property){
	std::auto_ptr<YAML::Node> node = get_node(property);
	map<string, string> ans;
	for(YAML::Iterator it = node->begin(); it != node->end(); ++it) {
		std::string key, value;
		it.first() >> key;
		it.second() >> value;
		ans[key] = value;
	}
	return ans;	
}

// Splits s by delimiter, and returns a vector with all the parts.
vector<string> Config::split(string s, char delimiter) {
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

#endif