#ifndef _DATASTRUCTURE_H
#define _DATASTRUCTURE_H

#include <utility>

class Point {
  public:
		Point(int xx = 0, int yy = 0, int ggrp = -1)
			: x(xx), y(yy), grp(ggrp) {}
		bool operator == (const Point& p1) {
			return x == p1.x && y == p1.y;
		}
		int x;
		int y;
		int grp;
};

class Edge {
	public:
		Edge(int pp1, int pp2, int w)
			: p1(pp1), p2(pp2), weight(w) {}
		int p1, p2;
		int weight;
};


#endif