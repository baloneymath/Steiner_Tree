#ifndef _STEINER_H
#define _STEINER_H

#include <vector>
#include <string>
#include <map>
#include <tuple>
#include <utility>
#include "datastructure.h"
using namespace std;

class Steiner {
	public:
		Steiner() :
			_name(""), _boundaryLeft(-1), _boundaryRight(-1),
			_boundaryTop(-1), _boundaryBottom(-1), 
			_MST_cost(0), _MRST_cost(0) {}
		~Steiner() {}

		void parse(const string& fileName);
		inline void plot();
		void solve();
		void outfile(const string& outfileName);
	private: // helper functions
		Point string2Point(string str);
		void init();
		void buildRSG();
		void buildMST();
		void buildRST();
		void addEdge(int p1, int p2);
		int  findSet(int pId);
		int  tarfind(int x);
		void tarunion(int x, int y);
		void tarjan(int x);
		void bruteforceLCA();
	private: // members
		string _name;
		int _boundaryLeft, _boundaryRight;
		int _boundaryTop, _boundaryBottom;
		unsigned _init_p;
		vector<Point> _points;
		vector<Edge> _edges;
		vector<unsigned> _MST;
		vector<bool> _mst_del;
		vector<vector<pair<int, int>>> _lca_queries; // p, e
		vector<vector<int>> _lca_answer_queries; // longest e
		// tarjan -------------
		vector<bool> _visit;
		vector<int>  _ancestor;
		vector<int>  _par;
		vector<int>  _rank;
		int _root;
		//----------------------
		vector<tuple<int, int, int, long>> _table;
		vector<bool> _table_illegal;
		vector<Edge>  _newE;

		long _MST_cost;
		long _MRST_cost;
};

#endif