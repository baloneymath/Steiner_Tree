#include <iostream>
#include <cassert>
#include "Steiner.h"
using namespace std;

int main(int argc, char** argv) {
	assert(argc == 2);
	ios::sync_with_stdio(false);
    cin.tie(0);
	string fileName = argv[1];
	Steiner steiner;
	steiner.parse(argv[1]);
	steiner.solve();
	return 0;
}