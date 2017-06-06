#ifndef _STEINER_H
#define _STEINER_H

#include <vector>
#include <string>
#include <map>
#include "datastructure.h"
using namespace std;

class Steiner {
	public:
		Steiner() :
			_name(""), _boundaryLeft(-1), _boundaryRight(-1),
			_boundaryTop(-1), _boundaryBottom(-1) {}
		~Steiner() {}

		void parse(const string& fileName);
		inline void plot();
		void solve();
	private: // helper functions
		Point string2Point(string str);
		void buildRSG();
		void buildMST();
		void buildRST();
		void addEdge(int p1, int p2);
		
	private: // members
		string _name;
		int _boundaryLeft, _boundaryRight;
		int _boundaryTop, _boundaryBottom;
		vector<Point> _points;
		vector<Edge> _edges;
		vector<Edge> _MST;
		vector<vector<int>> _groups;
};

#endif