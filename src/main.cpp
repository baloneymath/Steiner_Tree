#include <iostream>
#include <cassert>
#include <cstring>
#include <cstdio>
#include "Steiner.h"
using namespace std;

bool gDoplot, gOutfile;
string plotName, outfileName;
bool handleArgument(const int& argc, char** argv) {
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
			gDoplot = true;
			plotName = argv[++i];
		}
		else if (strcmp(argv[i] + 1, "out") == 0) {
			gOutfile = true;
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
	Steiner st;
	st.parse(argv[1]);
	st.solve();
	if (gDoplot) st.plot(plotName);
	if (gOutfile) st.outfile(outfileName);
	return 0;
}