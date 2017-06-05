#include <iostream>
#include <cassert>
#include "Steiner.h"
using namespace std;

int main(int argc, char** argv) {
	assert(argc == 2);
	string fileName = argv[1];
	Steiner steiner;
	steiner.parse(argv[1]);
	return 0;
}