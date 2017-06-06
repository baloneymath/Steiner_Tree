#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <algorithm>
#include <map>
#include <numeric>

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
inline string getFileName(const string& filePathName, bool getFile) {
  string retStr = filePathName;
  string::size_type pos = retStr.rfind("/");
  if (pos != string::npos)
    if (getFile) retStr = retStr.substr(pos + 1);
		else retStr = retStr.substr(0, pos);
  return retStr;
}
void Steiner::parse(const string& fileName) {
	_name = getFileName(fileName, true);
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
	_groups.resize(_points.size());
	for (unsigned i = 0; i < _points.size(); ++i) {
		ifs >> buf >> buf >> buf;
		_points[i] = string2Point(buf);
	}
	for (unsigned i = 0; i < _points.size(); ++i) {
		_points[i].grp = i;
	  _groups[i].push_back(i);
	}
}
void Steiner::addEdge(int p1, int p2) {
	if (p1 == p2) return;
	int weight = abs(_points[p1].x - _points[p2].x) +
					 abs(_points[p1].y - _points[p2].y);
	Edge e(p1, p2, weight);
	_edges.emplace_back(e);
}
void Steiner::solve() {
	// for (unsigned i = 0; i < _points.size(); ++i)
	// for (unsigned j = i; j < _points.size(); ++j) {
	// 	addEdge(i, j);
	// } 
	buildRSG();
	buildMST();
	buildRST();
	plot();
}
void Steiner::buildRSG() {
	vector<int> order1 ,order2;
	order1.resize(_points.size());
	iota(order1.begin(), order1.end(), 0);
	order2 = order1;
	sort(order1.begin(), order1.end(),
			[&] (int i1, int i2) {
				return _points[i1].x + _points[i1].y < _points[i2].x + _points[i2].y;
			});
	sort(order2.begin(), order2.end(),
			[&] (int i1, int i2) {
				return _points[i1].x - _points[i1].y < _points[i2].x - _points[i2].y;
			});
	multimap<int, int> A1, A2;
	for (auto& pId : order1) {
		Point& p = _points[pId];
		if (!A1.empty()) {
			auto it = A1.end();
			do {
				--it;
				Point& tmp = _points[(*it).second];
				if (p.y - tmp.y >= p.x - tmp.x && 
						p.y - tmp.y > 0 &&
						p.x - tmp.x > 0) {
					addEdge(pId, (*it).second);
					it = A1.erase(it);
				}
			} while (it != A1.begin());
		}
		if (!A2.empty()) {
			auto it = A2.end();
			do {
				--it;
				Point& tmp = _points[(*it).second];
				if (p.y - tmp.y < p.x - tmp.x && 
						p.y - tmp.y >= 0 &&
						p.x - tmp.x > 0) {
					addEdge(pId, (*it).second);
					it = A2.erase(it);
				}
			} while (it != A2.begin());
		}
		A1.emplace(p.x + p.y, pId);
		A2.emplace(p.x + p.y, pId);
	}
	A1.clear();
	A2.clear();
	for (auto& pId : order2) {
		Point& p = _points[pId];
		if (!A1.empty()) {
			auto it = A1.end();
			do {
				--it;
				Point& tmp = _points[(*it).second];
				if (tmp.y - p.y <= p.x - tmp.x && 
						p.y - tmp.y < 0 &&
						p.x - tmp.x > 0) {
					addEdge(pId, (*it).second);
					it = A1.erase(it);
				}
			} while (it != A1.begin());
		}
		if (!A2.empty()) {
			auto it = A2.end();
		  do {
				--it;
				Point& tmp = _points[(*it).second];
				if (tmp.y - p.y > p.x - tmp.x && 
						p.y - tmp.y < 0 &&
						p.x - tmp.x >= 0) {
					addEdge(pId, (*it).second);
					it = A2.erase(it);
				}
			} while (it != A2.begin());
		}
		A1.emplace(p.x + p.y, pId);
		A2.emplace(p.x + p.y, pId);
	}
	cerr << "RSG edge  : " << _edges.size() << endl;
}
void Steiner::buildMST() {
	sort(_edges.begin(), _edges.end(),
			[&] (Edge& e1, Edge& e2) {
				return e1.weight < e2.weight;
			});
	for (unsigned i = 0; i < _edges.size(); ++i) {
		Edge& e = _edges[i];
		Point& p1 = _points[e.p1];
		Point& p2 = _points[e.p2];
		if (p1.grp != p2.grp) {
			_MST.emplace_back(e);
			if (_groups[p1.grp].size() < _groups[p2.grp].size()) {
				for (int pId : _groups[p1.grp]) {
					_points[pId].grp = p2.grp;
					_groups[p2.grp].emplace_back(pId);
				}
			}
			else {
				for (int pId : _groups[p2.grp]) {
					_points[pId].grp = p1.grp;
					_groups[p1.grp].emplace_back(pId);
				}
			}
		}
	}
	sort(_MST.begin(), _MST.end(),
			[&] (Edge& e1, Edge& e2) {
				return e1.weight < e2.weight;
			});
	size_t cost = 0;
	for (auto& e : _MST) cost += e.weight;
	cerr << "MST length: " << cost << endl;
}
void Steiner::buildRST() {
	
}
void Steiner::plot() {
	string ofileName = _name + ".plt";
	fstream of(ofileName, ofstream::out);
	of << "set size ratio -1" << endl;
	of << "set nokey" << endl;
	of << "set xrange[" << -_boundaryRight * 0.1 << ":" 
		 << _boundaryRight * 1.1 << "]" << endl;
	of << "set yrange[" << -_boundaryTop * 0.1 << ":" 
		 << _boundaryTop * 1.1 << "]" << endl;
	int idx = 1;
	of << "set object " << idx++ << " rect from "
		 << _boundaryLeft << "," << _boundaryBottom << " to "
		 << _boundaryRight << "," << _boundaryTop << "fc rgb \"grey\" behind\n";
	// point
	for (unsigned i = 0; i < _points.size(); ++i) {
		of << "set object circle at first " << _points[i].x << ","
			 << _points[i].y << " radius char 0.3 fillstyle solid "
			 << "fc rgb \"red\" front\n";
	}
	// RSG
	for (unsigned i = 0; i < _edges.size(); ++i) {
		of << "set arrow " << idx++ << " from "
			 << _points[_edges[i].p1].x << "," << _points[_edges[i].p1].y << " to "
			 << _points[_edges[i].p2].x << "," << _points[_edges[i].p2].y 
			 << " nohead lc rgb \"white\" lw 1 front\n";
	}
	// MST
	for (unsigned i = 0; i < _MST.size(); ++i) {
		of << "set arrow " << idx++ << " from "
			 << _points[_MST[i].p1].x << "," << _points[_MST[i].p1].y << " to "
			 << _points[_MST[i].p2].x << "," << _points[_MST[i].p2].y 
			 << " nohead lc rgb \"blue\" lw 1 front\n";
	}
	of << "plot 1000000000" << endl;
	of << "pause -1 'Press any key'" << endl;
	of.close();
}