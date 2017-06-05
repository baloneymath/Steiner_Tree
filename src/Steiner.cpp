#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <algorithm>
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
	_pointOrder.resize(_points.size());
	for (unsigned i = 0; i < _points.size(); ++i) {
		ifs >> buf >> buf >> buf;
		_points[i] = string2Point(buf);
		_pointOrder.emplace_back(i);
	}
	sort(_pointOrder.begin(), _pointOrder.end(),
			[&] (int i1, int i2) {
				return _points[i1].first < _points[i2].first;
			});
}
void Steiner::buildSpanningGraph() {
	vector<Point> ps1 = _points, ps2 = ps1; // sort in x + y and x - y
	sort(ps1.begin(), ps1.end(),
			[&] (Point p1, Point p2) {
				return p1.first + p1.second < p2.first + p2.second;
			});
  sort(ps2.begin(), ps2.end(),
			[&] (Point p1, Point p2) {
				return p1.first - p1.second < p2.first - p2.second;
			});
}
void Steiner::findNeighbor(Point& p) {
	for (auto& pId : _pointOrder) {

	}
}