#include <iostream>
#include <fstream>
#include <string>
#include "Steiner.h"
using namespace std;

inline void split(string& str, const string& delim, vector<string>& v)
{
	size_t pos = 0;
	string token;
	v.clear();
	while ((pos = str.find(delim)) != string::npos) {
		token = str.substr(0, pos);
		str.erase(0, pos + delim.size());
		v.push_back(token);
	}
	v.push_back(str);
}
Point Steiner::string2Point(string str) {
	vector<string> tokens;
	split(str, ",", tokens);
	int x = stoi(tokens[0].substr(1));
	tokens[1].pop_back();
	int y = stoi(tokens[1]);
	return make_pair(x, y);
}
void Steiner::parse(const string& fileName) {
	fstream ifs(fileName, ifstream::in);
	string buf;
	Point p;
	ifs >> buf >> buf >> buf; buf.pop_back();
	p = string2Point(buf);
	_boundaryLeft = p.first;
	_boundaryBottom = p.second;
	ifs >> buf;
	p = string2Point(buf);
	_boundaryRight = p.first;
	_boundaryTop = p.second;
	
	ifs >> buf >> buf >> buf;
	_points.resize(stoi(buf));
	for (unsigned i = 0; i < _points.size(); ++i) {
		ifs >> buf >> buf >> buf;
		_points[i] = string2Point(buf);
	}
}
