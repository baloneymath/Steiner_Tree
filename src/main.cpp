#include <iostream>
#include <cassert>
#include <cstring>
#include <cstdio>
#include "Steiner.h"
using namespace std;

bool doplot, outfile;
string plotName, outfileName;
bool handleArgument( const int& argc, char** argv) {
	int i;
	if (argc < 2) {
		fprintf(stderr, "Usage: ./steiner <input> [-out <.out>] [-plot <.plt>]\n");
		return false;
	}
	while(i < argc) {
		if (strlen(argv[i]) <= 1) {
			++i;
			continue;
		}
		else if (strcmp(argv[i] + 1, "plot") == 0) {
			doplot = true;
			plotName = argv[++i];
		}
		else if (strcmp(argv[i] + 1, "out") == 0) {
			outfile = true;
			outfileName = argv[++i];
		}	
		++i;
	}
	return true;
}
int main(int argc, char** argv) {
	ios::sync_with_stdio(false);
  cin.tie(0);
	if (!handleArgument(argc, argv)) return -1;
	string fileName = argv[1];
	Steiner steiner;
	steiner.parse(argv[1]);
	steiner.solve();
	if (doplot) steiner.plot(plotName);
	if (outfile) steiner.outfile(outfileName);
	return 0;
}