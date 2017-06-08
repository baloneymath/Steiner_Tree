#ifndef _DATASTRUCTURE_H
#define _DATASTRUCTURE_H

#include <utility>
#include <vector>

class Point {
  public:
		Point(int xx = 0, int yy = 0, int ggrp = -1)
			: x(xx), y(yy), parent(-1) {}
		bool operator == (const Point& p1) {
			return x == p1.x && y == p1.y;
		}
		int x;
		int y;
		int parent;
		int grp;
		std::vector<int> neighbors;
};

class Edge {
	public:
		Edge(int pp1, int pp2, int w)
			: p1(pp1), p2(pp2), weight(w),
				parent(-1), left(-1), right(-1) {}
		bool operator == (const Edge& c1) {
			return p1 == c1.p1 && p2 == c1.p2 && weight == c1.weight;
		}
		int p1, p2;
		int weight;
		int parent;
		int grp;
		int left, right;
};


#endif