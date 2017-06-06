#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <algorithm>
#include <map>

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
	return Point(x, y);
}
void Steiner::parse(const string& fileName) {
	_name = fileName.substr(10);
	fstream ifs(fileName, ifstream::in);
	string buf;
	Point p;
	ifs >> buf >> buf >> buf; buf.pop_back();
	p = string2Point(buf);
	_boundaryLeft = p.x;
	_boundaryBottom = p.y;
	ifs >> buf;
	p = string2Point(buf);
	_boundaryRight = p.x;
	_boundaryTop = p.y;
	
	ifs >> buf >> buf >> buf;
	_points.resize(stoi(buf));
	_pointOrder.resize(_points.size());
	for (unsigned i = 0; i < _points.size(); ++i) {
		ifs >> buf >> buf >> buf;
		_points[i] = string2Point(buf);
		_pointOrder[i] = i;
	}
	sort(_pointOrder.begin(), _pointOrder.end(),
			[&] (int i1, int i2) {
				return _points[i1].x < _points[i2].x;
			});
	
}
void Steiner::solve() {
	buildSpanningGraph();
	plot();
	cerr << _edges.size() << endl;
}
void Steiner::buildSpanningGraph() {
	vector<int> order1 = _pointOrder;
	map<int, int> A1, A2;
	for (auto& pId : _pointOrder) {
		Point& p = _points[pId];
		if (!A1.empty()) {
			auto it = --A1.end();
			do {
				Point& tmp = _points[(*it).second];
				if (p.y - tmp.y >= p.x - tmp.x && 
						p.y - tmp.y >= 0 &&
						p.x - tmp.x >= 0) {
					addEdge(pId, (*it).second);
					it = A1.erase(it);
					break;
				}
			} while (it-- != A1.begin());
		}
		if (!A2.empty()) {
			auto it = --A2.end();
			do {
				Point& tmp = _points[(*it).second];
				if (p.y - tmp.y < p.x - tmp.x && 
						p.y - tmp.y >= 0 &&
						p.x - tmp.x >= 0) {
					addEdge(pId, (*it).second);
					it = A2.erase(it);
					break;
				}
			} while (it-- != A2.begin());
		}
		A1[p.x + p.y] = pId;
		A2[p.x + p.y] = pId;
	}
	A1.clear();
	A2.clear();
	for (auto& pId : _pointOrder) {
		Point& p = _points[pId];
		if (!A1.empty()) {
			auto it = --A1.end();
			do {
				Point& tmp = _points[(*it).second];
				if (tmp.y - p.y <= p.x - tmp.x && 
						tmp.y - p.y > 0 &&
						p.x - tmp.x >= 0) {
					addEdge(pId, (*it).second);
					it = A1.erase(it);
					break;
				}
			} while (it-- != A1.begin());
		}
		if (!A2.empty()) {
			auto it = --A2.end();
			do {
				Point& tmp = _points[(*it).second];
				if (tmp.y - p.y > p.x - tmp.x && 
						tmp.y - p.y > 0 &&
						p.x - tmp.x >= 0) {
					addEdge(pId, (*it).second);
					it = A2.erase(it);
					break;
				}

			} while (it-- != A2.begin());
		}
		A1[p.x - p.y] = pId;
		A2[p.x - p.y] = pId;
	}
}
void Steiner::addEdge(int p1, int p2) {
	if (p1 == p2) return;
	int weight = abs(_points[p1].x - _points[p2].x) +
					 abs(_points[p1].y - _points[p2].y);
	Edge e(p1, p2, weight);
	_edges.emplace_back(e);
}
void Steiner::buildMST() {
	sort(_edges.begin(), _edges.end(),
			[&] (Edge& e1, Edge& e2) {
				return e1.weight < e2.weight;
			});
	for (unsigned i = 0; i < _edges.size(); ++i) {
		
	}
}
void Steiner::plot() {
	string ofileName = _name + ".plt";
	fstream of(ofileName, ofstream::out);
	of << "set nokey" << endl;
	of << "set xrange[" << -_boundaryRight * 0.1 << ":" 
		 << _boundaryRight * 1.1 << "]" << endl;
	of << "set yrange[" << -_boundaryTop * 0.1 << ":" 
		 << _boundaryTop * 1.1 << "]" << endl;
	int idx = 1;
	of << "set object " << idx++ << " rect from "
		 << _boundaryLeft << "," << _boundaryBottom << " to "
		 << _boundaryRight << "," << _boundaryTop << "fc rgb \"grey\" behind\n";
	for (unsigned i = 0; i < _points.size(); ++i) {
		of << "set object circle at first " << _points[i].x << ","
			 << _points[i].y << " radius char 0.25 fillstyle empty "
			 << "border lc rgb '#aa1100' front\n";
	}
	for (unsigned i = 0; i < _edges.size(); ++i) {
		of << "set arrow " << idx++ << " from "
			 << _points[_edges[i].p1].x << "," << _points[_edges[i].p1].y << " to "
			 << _points[_edges[i].p2].x << "," << _points[_edges[i].p2].y 
			 << " nohead lc rgb \"white\" lw 1 front\n";
	}
	of << "plot 1000000000" << endl;
	of << "pause -1 'Press any key'" << endl;
	of.close();
}