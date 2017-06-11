#ifndef _STEINER_H
#define _STEINER_H

#include <vector>
#include <string>
#include <tuple>
#include "datastructure.h"

class Steiner {
  public:
    Steiner() :
      _name(""), _boundaryLeft(-1), _boundaryRight(-1),
      _boundaryTop(-1), _boundaryBottom(-1),
      _MST_cost(0), _MRST_cost(0) {}
    ~Steiner() {}

    void parse(const std::string& fileName);
    void solve();
    void plot(const std::string& plotName);
    void outfile(const std::string& outfileName);
  private: // helper functions
    // parse ---------------------
    void  addEdge(int p1, int p2);
    // solve ---------------------
    int  findSet(int pId);
    void init();
    void buildRSG();
    void buildMST();
    void buildRST();
    // LCA -----------------------
    int  tarfind(int x);
    void tarunion(int x, int y);
    void tarjanLCA(int x);
    void bruteforceLCA();
  private: // members
    std::string _name;
    int _boundaryLeft, _boundaryRight;
    int _boundaryTop, _boundaryBottom;
    int _init_p;
    int _root;
    std::vector<Point> _points;
    std::vector<Edge>  _edges;
    std::vector<int>   _MST;
    std::vector<bool>  _MST_del;
    std::vector<std::vector<int>>          _lca_place; // adj-list of index
    std::vector<std::tuple<int, int, int>> _lca_queries; // p, p, e
    std::vector<int>                       _lca_answer_queries; // longest e
    // tarjan -------------
    std::vector<bool> _visit;
    std::vector<int>  _ancestor;
    std::vector<int>  _par;
    std::vector<int>  _rank;
    //----------------------
    std::vector<std::tuple<int, int, int, int>> _table;
    std::vector<bool> _table_del;
    std::vector<Edge> _newE;

    long long _MST_cost;
    long long _MRST_cost;
    // plot ----------------
    std::vector<Edge> _init_edges;
    std::vector<int>  _init_MST;
};

#endif