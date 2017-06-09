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
	_init_p = _points.size();
	for (unsigned i = 0; i < _points.size(); ++i) {
		ifs >> buf >> buf >> buf;
		_points[i] = string2Point(buf);
	}
}
void Steiner::init() {
	_edges.clear();
	_MST.clear();
	_mst_del.clear();
	_lca_queries.clear();
	_lca_answer_queries.clear();
	_newE.clear();
	_table.clear();
	_table_illegal.clear();
}
void Steiner::solve() {
	for (unsigned iter = 0; iter < 4; ++iter) {
		init();
		buildRSG();
		buildMST();
		if (iter == 0) 
			for (unsigned eId : _MST) _MST_cost += _edges[eId].weight;
		buildRST();
	}
	for (auto& e : _newE) _MRST_cost += e.weight;
	for (unsigned eId : _MST) {
		if (_mst_del[eId]) continue;
		_MRST_cost += _edges[eId].weight;
	}
	cerr << endl;
	cerr << "RSG edge   : " << _edges.size() << endl;
	cerr << "MST length : " << _MST_cost << endl;
	cerr << "MRST length: " << _MRST_cost << endl;
	cerr << "Improve    : " 
			 << (float)(_MST_cost - _MRST_cost) / _MST_cost * 100 
			 << "%" << endl;
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
	_lca_queries.resize(_points.size());
	for (unsigned i = 0; i < _edges.size(); ++i) {
		Edge& e = _edges[i];
		int head1 = findSet(e.p1);
		int head2 = findSet(e.p2);
		if (head1 != head2) {
			for (auto& w : _points[e.p1].neighbors) {
				if (w == e.p2) continue;
				if (head1 == findSet(w)) _lca_queries[w].emplace_back(e.p1, i);
				else 										 _lca_queries[w].emplace_back(e.p2, i);
			}
			for (auto& w : _points[e.p2].neighbors) {
				if (w == e.p1) continue;
				if (head2 == findSet(w)) _lca_queries[w].emplace_back(e.p2, i);
				else 										 _lca_queries[w].emplace_back(e.p1, i);
			}
			if (head1 >= _edges.size()) _points[e.p1].parent = i;
			else _edges[head1].parent = i;
			if (head2 >= _edges.size()) _points[e.p2].parent = i;
			else _edges[head2].parent = i;
			_edges[i].left = head1;
			_edges[i].right = head2;
			_MST.emplace_back(i);
		}
	}
	_root = findSet(0);
}
int Steiner::find(int x) {
	if (x != _grp[x]) _grp[x] = find(_grp[x]);
	return _grp[x];
}
void Steiner::tarjan(int x) {
	if (_visit[x]) return;
	if (x >= _edges.size()) cerr << "p" << x - _edges.size() << endl;
	else cerr << "e" << x << endl;
	_ancestor[x] = x;
	if (x < _edges.size()) {
		tarjan(_edges[x].left);
		_grp[_edges[x].left] = _grp[x];
		_ancestor[find(x)] = x;
		tarjan(_edges[x].right);
		_grp[_edges[x].right] = _grp[x];
		_ancestor[find(x)] = x;
	}
	_visit[x] = true;
	if (x >= _edges.size()) {
		int u = x - _edges.size();
		for (unsigned i = 0; i < _lca_queries[u].size(); ++i) {	
			int v = get<0>(_lca_queries[u][i]) + _edges.size();
			if (_visit[v])
				_lca_answer_queries[u][i] = _ancestor[find(v)];
		}
	}
}
void Steiner::buildRST() {
	_visit.resize(_edges.size() + _points.size());
	_ancestor.resize(_edges.size() + _points.size());
	_grp.resize(_edges.size() + _points.size());
	iota(_grp.begin(), _grp.end(), 0);
	_lca_answer_queries.resize(_lca_queries.size());
	for (unsigned i = 0; i < _lca_queries.size(); ++i)
		_lca_answer_queries[i].resize(_lca_queries[i].size());
	//tarjan(_root);
	for (unsigned i = 0; i < _lca_queries.size(); ++i) {
		for (unsigned j = 0; j < _lca_queries[i].size(); ++j) {
			int u = i, v = get<0>(_lca_queries[i][j]);
			int puId = _points[u].parent;
			int pvId = _points[v].parent;
			Edge pu = _edges[puId];
			Edge pv = _edges[pvId];
			vector<int> pus, pvs;
			pus.emplace_back(puId);
			pvs.emplace_back(pvId);
			while (pu.parent != puId) {
				pus.emplace_back(pu.parent);
				puId = pu.parent;
				pu = _edges[puId];
			}
			while (pv.parent != pvId) {
				pvs.emplace_back(pv.parent);
				pvId = pv.parent;
				pv = _edges[pvId];
			}
			auto it = find_first_of(pus.begin(), pus.end(), pvs.begin(), pvs.end());
			_lca_answer_queries[i][j] = *it;
		}
	}
	for (unsigned i = 0; i < _lca_queries.size(); ++i) {
		for (unsigned j = 0; j < _lca_queries[i].size(); ++j) {
			int p = i;
			int ae = get<1>(_lca_queries[i][j]);
			int de = _lca_answer_queries[i][j];
			Point& pnt = _points[p];
			Edge& add_e = _edges[ae];
			Edge& del_e = _edges[de];
			int gain = del_e.weight;
			int mxx = max(_points[add_e.p1].x, _points[add_e.p2].x);
		  int mnx = min(_points[add_e.p1].x, _points[add_e.p2].x);
	   	int mxy = max(_points[add_e.p1].y, _points[add_e.p2].y);
	  	int mny = min(_points[add_e.p1].y, _points[add_e.p2].y);
			if (pnt.x < mnx)      gain -= mnx - pnt.x;
			else if (pnt.x > mxx) gain -= pnt.x - mxx;
			if (pnt.y < mny)      gain -= mny - pnt.y;
			else if (pnt.y > mxy) gain -= pnt.y - mxy;
			if (gain > 0 && gain != del_e.weight) 
				_table.emplace_back(p, ae, de, gain);
		}
	}
	sort(_table.begin(), _table.end(),
			[&] (tuple<int, int, int, int> t1, tuple<int, int, int, int> t2) {
				return get<3>(t1) > get<3>(t2);
			});
	_table.resize(unique(_table.begin(), _table.end()) - _table.begin());
	//for (unsigned i = 0; i < _table.size(); ++i) {
	//	cerr << get<0>(_table[i]) << " (" << _edges[get<1>(_table[i])].p1 << ","
	//			 << _edges[get<1>(_table[i])].p2 << ")"
	//			 << " (" << _edges[get<2>(_table[i])].p1 << ","
	//			 << _edges[get<2>(_table[i])].p2 << ") "
	//			 << get<3>(_table[i]) << endl;
	//}
	//cerr << "table size: " << _table.size() << endl;
	_mst_del.resize(_edges.size());
	for (unsigned i = 0; i < _table.size(); ++i) {
		auto it = std::find(_table_illegal.begin(), _table_illegal.end(), i);
		if (it != _table_illegal.end()) continue;
		_mst_del[get<1>(_table[i])] = true;
		_mst_del[get<2>(_table[i])] = true;
		Point p = _points[get<0>(_table[i])];
		Edge& add_e = _edges[get<1>(_table[i])];
		Edge& del_e = _edges[get<2>(_table[i])];
		int mxx = max(_points[add_e.p1].x, _points[add_e.p2].x);
		int mnx = min(_points[add_e.p1].x, _points[add_e.p2].x);
		int mxy = max(_points[add_e.p1].y, _points[add_e.p2].y);
		int mny = min(_points[add_e.p1].y, _points[add_e.p2].y);
		int sx = p.x, sy = p.y;
		if (p.x < mnx) sx = mnx;
		else if (p.x > mxx) sx = mxx;
		if (p.y < mny) sy = mny;
		else if (p.y > mxy) sy = mxy;
		if (sx != p.x || sy != p.y) {
			int pId = get<0>(_table[i]);
			int new_pId = _points.size();
			Point new_p = Point(sx, sy);
			_points.emplace_back(new_p);
			int weight = abs(p.x - new_p.x) + abs(p.y - new_p.y);
			int weight1 = abs(_points[add_e.p1].x - new_p.x) + 
										abs(_points[add_e.p1].y - new_p.y);
			int weight2 = abs(_points[add_e.p2].x - new_p.x) + 
										abs(_points[add_e.p2].y - new_p.y);
			_newE.emplace_back(Edge(new_pId, pId, weight));
			_newE.emplace_back(Edge(new_pId, add_e.p1, weight1));
			_newE.emplace_back(Edge(new_pId, add_e.p2, weight2));
		}
		for (unsigned j = i; j < _table.size(); ++j)
			if (get<1>(_table[j]) == get<2>(_table[i]) ||
			    get<2>(_table[j]) == get<2>(_table[i]) ||
					get<2>(_table[j]) == get<1>(_table[i]) ||
					get<1>(_table[j]) == get<1>(_table[i])) {
				_table_illegal.emplace_back(j);
			}
	}

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
	for (unsigned i = 0; i < _init_p; ++i) {
		of << "set object circle at first " << _points[i].x << ","
			 << _points[i].y << " radius char 0.3 fillstyle solid "
			 << "fc rgb \"red\" front\n";
		//of << "set label \"p" << i << "\" at " << _points[i].x + 2<< ","
		//	 << _points[i].y + 2<< endl; 
	}
	// RSG
	for (unsigned i = 0; i < _edges.size(); ++i) {
		of << "set arrow " << idx++ << " from "
			 << _points[_edges[i].p1].x << "," << _points[_edges[i].p1].y << " to "
			 << _points[_edges[i].p2].x << "," << _points[_edges[i].p2].y 
			 << " nohead lc rgb \"white\" lw 1 front\n";
		//of << "set label \"e" << i << "\" at " 
		//	 << (_points[_edges[i].p1].x + _points[_edges[i].p2].x) / 2 << ","
		//	 << (_points[_edges[i].p1].y + _points[_edges[i].p2].y) / 2 << endl;
	}
	// MST
	for (unsigned i = 0; i < _MST.size(); ++i) {
		of << "set arrow " << idx++ << " from "
			 << _points[_edges[_MST[i]].p1].x << "," 
			 << _points[_edges[_MST[i]].p1].y << " to "
			 << _points[_edges[_MST[i]].p2].x << "," 
			 << _points[_edges[_MST[i]].p2].y 
			 << " nohead lc rgb \"blue\" lw 1 front\n";
	}
	// s-point
	for (unsigned i = _init_p; i < _points.size(); ++i) {
		of << "set object circle at first " << _points[i].x << ","
			 << _points[i].y << " radius char 0.3 fillstyle solid "
			 << "fc rgb \"yellow\" front\n";
	}
	// RST
	for (unsigned i = 0; i < _MST.size(); ++i) {
		if (_mst_del[_MST[i]]) continue;
		of << "set arrow " << idx++ << " from "
		   << _points[_edges[_MST[i]].p1].x << "," 
			 << _points[_edges[_MST[i]].p1].y << " to "
			 << _points[_edges[_MST[i]].p2].x << "," 
			 << _points[_edges[_MST[i]].p1].y
			 << " nohead lc rgb \"black\" lw 1.5 front\n";
		of << "set arrow " << idx++ << " from "
		   << _points[_edges[_MST[i]].p2].x << "," 
			 << _points[_edges[_MST[i]].p1].y << " to "
			 << _points[_edges[_MST[i]].p2].x << "," 
			 << _points[_edges[_MST[i]].p2].y
			 << " nohead lc rgb \"black\" lw 1.5 front\n";
	}
	for (unsigned i = 0; i < _newE.size(); ++i) {
		of << "set arrow " << idx++ << " from "
		   << _points[_newE[i].p1].x << "," << _points[_newE[i].p1].y << " to "
			 << _points[_newE[i].p2].x << "," << _points[_newE[i].p1].y
			 << " nohead lc rgb \"black\" lw 1.5 front\n";
	  of << "set arrow " << idx++ << " from "
		   << _points[_newE[i].p2].x << "," << _points[_newE[i].p1].y << " to "
			 << _points[_newE[i].p2].x << "," << _points[_newE[i].p2].y
			 << " nohead lc rgb \"black\" lw 1.5 front\n";
	}
	of << "plot 1000000000" << endl;
	of << "pause -1 'Press any key'" << endl;
	of.close();
}
void Steiner::outfile(const string& outfileName) {
	ofstream of(outfileName, ofstream::out);
	of << "NumRoutedPins = " << _init_p << endl;
	of << "WireLength = " << _MRST_cost << endl;
	for (unsigned i = 0; i < _MST.size(); ++i) {
		if (_mst_del[_MST[i]]) continue;
		Point& p1 = _points[_edges[_MST[i]].p1];
		Point& p2 = _points[_edges[_MST[i]].p2];
		if (p1.x != p2.x) {
			of << "H-line "
				 << "(" << p1.x << "," << p1.y << ") "
				 << "(" << p2.x << "," << p1.y << ")"
				 << endl; 
		}
		if (p1.y != p2.y) {
			of << "V-line "
				 << "(" << p2.x << "," << p1.y << ") "
				 << "(" << p2.x << "," << p2.y << ")"
				 << endl; 
		}
	}
	for (unsigned i = 0; i < _newE.size(); ++i) {
		Point& p1 = _points[_newE[i].p1];
		Point& p2 = _points[_newE[i].p2];
		if (p1.x != p2.x) {
			of << "H-line "
				 << "(" << p1.x << "," << p1.y << ") "
				 << "(" << p2.x << "," << p1.y << ")"
				 << endl; 
		}
		if (p1.y != p2.y) {
			of << "V-line "
				 << "(" << p2.x << "," << p1.y << ") "
				 << "(" << p2.x << "," << p2.y << ")"
				 << endl; 
		}
	}
	of.close();
}