#include <iostream>
#include <cassert>
#include <cstring>
#include <cstdio>
#include "util.h"
#include "Steiner.h"
using namespace std;

bool gDoplot, gOutfile;
string plotName, outfileName;
bool handleArgument(const int& argc, char** argv) {
  int i = 0;
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
  TimeUsage timer;
  if (!handleArgument(argc, argv)) return -1;
  string fileName = argv[1];
  Steiner st;
  st.parse(argv[1]);
  st.solve();
#ifdef VERBOSE
  timer.showUsage("solve", TimeUsage::PARTIAL);
  timer.start(TimeUsage::PARTIAL);
#endif
  if (gDoplot) {
    st.plot(plotName);
#ifdef VERBOSE    
    timer.showUsage("plot", TimeUsage::PARTIAL);
    timer.start(TimeUsage::PARTIAL);
#endif
  }
  if (gOutfile) {
    st.outfile(outfileName);
#ifdef VERBOSE
    timer.showUsage("outfile", TimeUsage::PARTIAL);
    timer.start(TimeUsage::PARTIAL);
#endif
  }
  timer.showUsage("Steiner", TimeUsage::FULL);
  return 0;
}