#ifndef _STEINER_H
#define _STEINER_H

#include <vector>
#include <string>
#include "datastructure.h"
using namespace std;

class Steiner {
	public:
		Steiner() :
			_name(""), _boundaryLeft(-1), _boundaryRight(-1),
			_boundaryTop(-1), _boundaryBottom(-1) {}
		~Steiner() {}

		void parse(const string& fileName);
	private: // helper functions
		Point string2Point(string str);
		void buildSpanningGraph();
		void findNeighbor(Point& p);
	private: // members
		string _name;
		int _boundaryLeft, _boundaryRight;
		int _boundaryTop, _boundaryBottom;
		vector<int> _pointOrder; // in x
		vector<Point> _points;
};

#endif