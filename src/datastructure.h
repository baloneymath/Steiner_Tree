#ifndef _DATASTRUCTURE_H
#define _DATASTRUCTURE_H

#include <vector>

class Point {
  public:
    Point(int xx = 0, int yy = 0)
      : x(xx), y(yy), parent(-1) {}
    ~Point() {}
    bool operator == (const Point& p1) {
      return x == p1.x && y == p1.y;
    }
    int x;
    int y;
    int parent;
    std::vector<int> neighbors;
};

class Edge {
  public:
    Edge(int pp1, int pp2, int w)
      : p1(pp1), p2(pp2), weight(w),
        parent(-1), left(-1), right(-1) {}
    ~Edge() {}
    bool operator == (const Edge& c1) {
      return p1 == c1.p1 && p2 == c1.p2;
    }
    int p1, p2;
    int weight;
    int parent;
    int left, right;
};


#endif