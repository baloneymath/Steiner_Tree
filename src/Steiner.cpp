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
	for (unsigned i = 0; i < _points.size(); ++i) {
		ifs >> buf >> buf >> buf;
		_points[i] = string2Point(buf);
	}
}
void Steiner::solve() {
	buildRSG();
	buildMST();
	buildRST();
	plot();
}
void Steiner::addEdge(int p1, int p2) {
	if (p1 == p2) return;
	int weight = abs(_points[p1].x - _points[p2].x) +
					 abs(_points[p1].y - _points[p2].y);
	Edge e(p1, p2, weight);
	_edges.emplace_back(e);
	_points[p1].neighbors.emplace_back(p2);
	_points[p2].neighbors.emplace_back(p1);
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
					break;
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
					break;
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
					break;
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
					break;
				}
			} while (it != A2.begin());
		}
		A1.emplace(p.x - p.y, pId);
		A2.emplace(p.x - p.y, pId);
	}
	cerr << "RSG edge  : " << _edges.size() << endl;
}
int Steiner::findSet(int pId) {
	if (_points[pId].parent >= _edges.size()) return _points[pId].parent;
	else {
		Edge e = _edges[_points[pId].parent];
		int ans = _points[pId].parent;
		while (e.parent != ans) {
			ans = e.parent;
			e = _edges[e.parent];
		}
		return ans;
	}
}
void Steiner::buildMST() {
	sort(_edges.begin(), _edges.end(),
			[&] (Edge& e1, Edge& e2) {
				return e1.weight < e2.weight;
			});
	for (unsigned i = 0; i < _points.size(); ++i)
		_points[i].parent = i + _edges.size();
	for (unsigned i = 0; i < _edges.size(); ++i)
		_edges[i].parent = i;
	size_t cost = 0;
	for (unsigned i = 0; i < _edges.size(); ++i) {
		Edge& e = _edges[i];
		int head1 = findSet(e.p1);
		int head2 = findSet(e.p2);
		if (head1 != head2) {
			unsigned cnt = _MST.size();
			_MST.emplace_back(e);
			cost += e.weight;
			for (auto& w : _points[e.p1].neighbors) {
				if (w == e.p2) continue;
				if (head1 == findSet(w)) _lca_queries.emplace_back(w, e.p1, cnt);
				else _lca_queries.emplace_back(w, e.p2, cnt);
			}
			for (auto& w : _points[e.p2].neighbors) {
				if (w == e.p1) continue;
				if (head2 == findSet(w)) _lca_queries.emplace_back(w, e.p2, cnt);
				else _lca_queries.emplace_back(w, e.p1, cnt);
			}
			if (head1 >= _edges.size()) _points[e.p1].parent = i;
			else _edges[head1].parent = i;
			if (head2 >= _edges.size()) _points[e.p2].parent = i;
			else _edges[head2].parent = i;
			_edges[i].left = head1;
			_edges[i].right = head2;
		}
	}
	_root = findSet(0);
	cerr << "MST length: " << cost << endl;
}
int Steiner::find(int x) {
	if (x != _grp[x]) _grp[x] = find(_grp[x]);
	return _grp[x];
}
void Steiner::tarjan(int x) {
	if (_visit[x]) return;
	_visit[x] = true;
	_ancestor[x] = x;
	if (x < _edges.size()) {
		tarjan(_edges[x].left);
		tarjan(_edges[x].right);
		_ancestor[find(x)] = x;
		for (unsigned i = 0; i < _lca_queries.size(); ++i) {
			auto& query = _lca_queries[i];
			int v = get<1>(query), v2 = get<2>(query);
			if (get<2>(query) == x && _visit[v]) {
				_lca_answer_queries.emplace_back(v, x, _ancestor[find(v)]);
			}
			else if (get<1>(query) == x && _visit[v2]) {
				_lca_answer_queries.emplace_back(x, v2, _ancestor[find(v2)]);
			}
		}
	}
}
void Steiner::buildRST() {
	_visit.resize(_edges.size() + _points.size());
	_ancestor.resize(_edges.size() + _points.size());
	_lca_answer_queries.reserve(_lca_queries.size());
	_grp.resize(_edges.size() + _points.size());
	iota(_grp.begin(), _grp.end(), 0);
	tarjan(_root);
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