#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <map>
#include <set>
#include <numeric>

#include "util.h"
#include "Steiner.h"
using namespace std;

inline void split(string& str, const string& delim, vector<string>& v) {
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
inline Point string2Point(string str) {
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
  if (pos != string::npos) {
    if (getFile) retStr = retStr.substr(pos + 1);
    else retStr = retStr.substr(0, pos);
  }
  return retStr;
}
void Steiner::parse(const string& fileName) {
  _name = getFileName(fileName, true);
  ifstream ifs(fileName, ifstream::in);
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
  ifs.close();
}
void Steiner::init() {
  _edges.clear();
  _grps.clear();
  _p_set.clear();
  _MST.clear();
  _MST_del.clear();
  _lca_place.clear();
  _lca_queries.clear();
  _lca_answer_queries.clear();
  _visit.clear();
  _ancestor.clear();
  _par.clear();
  _rank.clear();
  _newE.clear();
  _table.clear();
  _table_del.clear();
  _table_place.clear();
}
extern bool gDoplot;
void Steiner::solve() {
#ifdef VERBOSE
  TimeUsage timer;
#endif
  buildRSG();
#ifdef VERBOSE
  timer.showUsage("buildRSG", TimeUsage::PARTIAL);
  timer.start(TimeUsage::PARTIAL);
#endif
  buildMST();
#ifdef VERBOSE
  timer.showUsage("buildMST", TimeUsage::PARTIAL);
  timer.start(TimeUsage::PARTIAL);
#endif
  if (gDoplot) {
    _init_edges = _edges;
    _init_MST = _MST;
  }
  for (int eId : _MST) _MST_cost += _edges[eId].weight;
  buildRST();
#ifdef VERBOSE
  timer.showUsage("buildRST", TimeUsage::PARTIAL);
  timer.start(TimeUsage::PARTIAL);
#endif
  unsigned numIter = 2;
  if (_init_p >= 10) numIter = 2;
  if (_init_p >= 100) numIter = 3;
  if (_init_p >= 500) numIter = 4;
  for (unsigned iter = 1; iter < numIter; ++iter) {
    init();
    buildRSG();
#ifdef VERBOSE
  timer.showUsage("buildRSG", TimeUsage::PARTIAL);
  timer.start(TimeUsage::PARTIAL);
#endif
    buildMST();
#ifdef VERBOSE
  timer.showUsage("buildMST", TimeUsage::PARTIAL);
  timer.start(TimeUsage::PARTIAL);
#endif
    buildRST();
#ifdef VERBOSE
  timer.showUsage("buildRST", TimeUsage::PARTIAL);
  timer.start(TimeUsage::PARTIAL);
#endif
  }
  for (auto& e : _newE) _MRST_cost += e.weight;
  for (int eId : _MST) {
    if (_MST_del[eId]) continue;
    _MRST_cost += _edges[eId].weight;
  }
  cerr << _name << endl;
  cerr << "RSG edge   : " << _edges.size() << endl;
  cerr << "MST length : " << _MST_cost << endl;
  cerr << "MRST length: " << _MRST_cost << endl;
  cerr << "Improvement: "
       << (double)(_MST_cost - _MRST_cost) / _MST_cost * 100
       << "%" << endl;
}
void Steiner::addEdge(int p1, int p2) {
  if (p1 == p2) return;
  int weight = abs(_points[p1].x - _points[p2].x) +
                abs(_points[p1].y - _points[p2].y);
  _edges.emplace_back(Edge(p1, p2, weight));
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
  vector<int> A1, A2;
  for (int pId : order1) {
    Point& p = _points[pId];
    if (!A1.empty()) {
      auto it = A1.end();
      do {
        --it;
        Point& tmp = _points[*it];
        if (p.y - tmp.y >= p.x - tmp.x &&
            p.y - tmp.y > 0 &&
            p.x - tmp.x > 0) {
          addEdge(pId, *it);
          it = A1.erase(it);
          //break;
        }
      } while (it != A1.begin());
    }
    if (!A2.empty()) {
      auto it = A2.end();
      do {
        --it;
        Point& tmp = _points[*it];
        if (p.y - tmp.y < p.x - tmp.x &&
            p.y - tmp.y >= 0 &&
            p.x - tmp.x > 0) {
          addEdge(pId, *it);
          it = A2.erase(it);
          //break;
        }
      } while (it != A2.begin());
    }
    A1.emplace_back(pId);
    A2.emplace_back(pId);
  }
  A1.clear();
  A2.clear();
  for (int pId : order2) {
    Point& p = _points[pId];
    if (!A1.empty()) {
      auto it = A1.end();
      do {
        --it;
        Point& tmp = _points[*it];
        if (tmp.y - p.y <= p.x - tmp.x &&
            p.y - tmp.y < 0 &&
            p.x - tmp.x > 0) {
          addEdge(pId, *it);
          it = A1.erase(it);
          //break;
        }
      } while (it != A1.begin());
    }
    if (!A2.empty()) {
      auto it = A2.end();
      do {
        --it;
        Point& tmp = _points[*it];
        if (tmp.y - p.y > p.x - tmp.x &&
            p.y - tmp.y < 0 &&
            p.x - tmp.x >= 0) {
          addEdge(pId, *it);
          it = A2.erase(it);
          //break;
        }
      } while (it != A2.begin());
    }
    A1.emplace_back(pId);
    A2.emplace_back(pId);
  }

}
unsigned Steiner::findSet(int pId) {
  return _p_set[pId];
}
void Steiner::buildMST() {
  sort(_edges.begin(), _edges.end(),
      [] (Edge e1, Edge e2) {
        return e1.weight < e2.weight;
      });
  _p_set.resize(_points.size());
  iota(_p_set.begin(), _p_set.end(), _edges.size());
  _grps.resize(_edges.size());
  _lca_place.resize(_points.size());
  for (unsigned i = 0; i < _edges.size(); ++i) {
    Edge& e = _edges[i];
    unsigned head1 = findSet(e.p1);
    unsigned head2 = findSet(e.p2);
    if (head1 != head2) {
      set<int> neighbors;
      for (int n : _points[e.p1].neighbors) neighbors.emplace(n);
      for (int n : _points[e.p2].neighbors) neighbors.emplace(n);
      neighbors.erase(e.p1);
      neighbors.erase(e.p2);
      for (int w : neighbors) {
        if (head1 == findSet(w)) {
          _lca_place[w].emplace_back(_lca_queries.size());
          _lca_place[e.p1].emplace_back(_lca_queries.size());
          _lca_queries.emplace_back(w, e.p1, i);
        }
        else {
          _lca_place[w].emplace_back(_lca_queries.size());
          _lca_place[e.p2].emplace_back(_lca_queries.size());
          _lca_queries.emplace_back(w, e.p2, i);
        }
      }
      if (head1 < _edges.size()) {
        for (int p : _grps[head1]) {
          _p_set[p] = i;
          _grps[i].emplace_back(p);
        }
        _grps[head1].clear();
      }
      else {
        int pId = head1 - _edges.size();
        _p_set[pId] = i;
        _grps[i].emplace_back(pId);
      }
      if (head2 < _edges.size()) {
        for (int p : _grps[head2]) {
          _p_set[p] = i;
          _grps[i].emplace_back(p);
        }
        _grps[head2].clear();
      }
      else {
        int pId = head2 - _edges.size();
        _p_set[pId] = i;
        _grps[i].emplace_back(pId);
      }
      e.left = head1;
      e.right = head2;
      _MST.emplace_back(i);
      if (_grps[i].size() == _points.size()) break;
    }
  }
  _root = findSet(0);
}
int Steiner::tarfind(int x) {
  return x == _par[x] ? _par[x] : (_par[x] = tarfind(_par[x]));
}
void Steiner::tarunion(int x, int y) {
  int xroot = tarfind(x);
  int yroot = tarfind(y);
  if (_rank[xroot] > _rank[yroot]) _par[yroot] = xroot;
  else if (_rank[xroot] < _rank[yroot]) _par[xroot] = yroot;
  else {
    _par[yroot] = xroot;
    ++_rank[xroot];
  }
}
void Steiner::tarjanLCA(int x) {
  _par[x] = x;
  _ancestor[x] = x;
  if (x < (int)_edges.size()) {
    tarjanLCA(_edges[x].left);
    tarunion(x, _edges[x].left);
    _ancestor[tarfind(x)] = x;
    tarjanLCA(_edges[x].right);
    tarunion(x, _edges[x].right);
    _ancestor[tarfind(x)] = x;
  }
  _visit[x] = true;
  if (x >= (int)_edges.size()) {
    int u = x - _edges.size();
    for (unsigned i = 0; i < _lca_place[u].size(); ++i) {
      int which = _lca_place[u][i];
      int v = get<0>(_lca_queries[which]) == u ?
              get<1>(_lca_queries[which]) : get<0>(_lca_queries[which]);
      v += _edges.size();
      if (_visit[v]) _lca_answer_queries[which] = _ancestor[tarfind(v)];
    }
  }
}
void Steiner::buildRST() {
  _lca_answer_queries.resize(_lca_queries.size());
  _visit.resize(_edges.size() + _points.size());
  _ancestor.resize(_edges.size() + _points.size());
  _par.resize(_edges.size() + _points.size());
  _rank.resize(_edges.size() + _points.size());
  tarjanLCA(_root);
  _table.reserve(_lca_queries.size());
  _table_place.resize(_edges.size());
  for (unsigned i = 0; i < _lca_queries.size(); ++i) {
    int p = get<0>(_lca_queries[i]);
    int ae = get<2>(_lca_queries[i]);
    int de = _lca_answer_queries[i];
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
    if (gain > 0) _table.emplace_back(p, ae, de, gain);
  }
  sort(_table.begin(), _table.end(),
      [] (tuple<int, int, int, int> t1, tuple<int, int, int, int> t2) {
        return get<3>(t1) > get<3>(t2);
      });
  for (unsigned i = 0; i < _table.size(); ++i) {
    _table_place[get<1>(_table[i])].emplace_back(i);
    _table_place[get<2>(_table[i])].emplace_back(i);
  }
#ifdef DEBUG
  for (unsigned i = 0; i < _table.size(); ++i) {
    cerr << get<0>(_table[i]) << " (" << _edges[get<1>(_table[i])].p1 << ","
         << _edges[get<1>(_table[i])].p2 << ")"
         << " (" << _edges[get<2>(_table[i])].p1 << ","
         << _edges[get<2>(_table[i])].p2 << ") "
         << get<3>(_table[i]) << endl;
  }
#endif
  _MST_del.resize(_edges.size());
  _table_del.resize(_table.size());
  for (unsigned i = 0; i < _table.size(); ++i) {
    if (_table_del[i]) continue;
    _MST_del[get<1>(_table[i])] = true;
    _MST_del[get<2>(_table[i])] = true;
    Point p = _points[get<0>(_table[i])];
    Edge& add_e = _edges[get<1>(_table[i])];
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
    for (int pos : _table_place[get<1>(_table[i])]) _table_del[pos] = true;
    for (int pos : _table_place[get<2>(_table[i])]) _table_del[pos] = true;
  }

}
void Steiner::plot(const string& plotName) {
  string ofileName = plotName;
  ofstream of(ofileName, ofstream::out);
  of << "set size ratio -1" << endl;
  of << "set nokey" << endl;
  of << "set xrange["
     << (_boundaryRight - _boundaryLeft) * -0.05 << ":"
     << (_boundaryRight - _boundaryLeft) * 1.05 << "]" << endl;
  of << "set yrange["
     << (_boundaryTop - _boundaryBottom) * -0.05 << ":"
     << (_boundaryTop - _boundaryBottom) * 1.05 << "]" << endl;
  int idx = 1;
  of << "set object " << idx++ << " rect from "
     << _boundaryLeft << "," << _boundaryBottom << " to "
     << _boundaryRight << "," << _boundaryTop << "fc rgb \"grey\" behind\n";
  // point
  for (int i = 0; i < _init_p; ++i) {
    of << "set object circle at first " << _points[i].x << ","
       << _points[i].y << " radius char 0.3 fillstyle solid "
       << "fc rgb \"red\" front\n";
  }
  // RSG
  for (unsigned i = 0; i < _init_edges.size(); ++i) {
    Point& p1 = _points[_init_edges[i].p1];
    Point& p2 = _points[_init_edges[i].p2];
    of << "set arrow " << idx++ << " from "
       << p1.x << "," << p1.y << " to "
       << p2.x << "," << p2.y
       << " nohead lc rgb \"white\" lw 1 front\n";
  }
  // MST
  for (unsigned i = 0; i < _init_MST.size(); ++i) {
    Point& p1 = _points[_init_edges[_init_MST[i]].p1];
    Point& p2 = _points[_init_edges[_init_MST[i]].p2];
    of << "set arrow " << idx++ << " from "
       << p1.x << "," << p1.y << " to "
       << p2.x << "," << p2.y
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
    if (_MST_del[_MST[i]]) continue;
    Point& p1 = _points[_edges[_MST[i]].p1];
    Point& p2 = _points[_edges[_MST[i]].p2];
    if (p1.x != p2.x) {
      of << "set arrow " << idx++ << " from "
        << p1.x << "," << p1.y << " to "
        << p2.x << "," << p1.y
        << " nohead lc rgb \"black\" lw 1.5 front\n";
    }
    if (p1.y != p2.y) {
      of << "set arrow " << idx++ << " from "
        << p2.x << "," << p1.y << " to "
        << p2.x << "," << p2.y
        << " nohead lc rgb \"black\" lw 1.5 front\n";
    }
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
    if (_MST_del[_MST[i]]) continue;
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