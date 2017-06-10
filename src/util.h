#ifndef _UTIL_H
#define _UTIL_H

#include <cstdlib> // atof
#include <cassert> // assert
#include <cmath> // fabs
#include <iostream> // cerr, ostream
#include <fstream> // ifstream
#include <vector>
#include <sstream> // stringstream
#include <sys/resource.h>
#include <sys/time.h>
#include <limits> // numeric_limits

constexpr double TIME_SCALE   = 1000000.0;
constexpr double MEMORY_SCALE = 1024.0;

class TimeUsage {
	public:
		TimeUsage() { start(FULL); start(PARTIAL); }
		struct TimeState {
			TimeState(double r = 0, double u = 0, double s = 0)
				: rTime(r), uTime(u), sTime(s) {}
			double rTime, uTime, sTime; //real, user, system
		};
		enum TimeType { FULL, PARTIAL };
		void start(TimeType type) { 
			(type == FULL) ? checkUsage(tStart_) : checkUsage(pStart_); 
		}
		void showUsage(const std::string& comment, TimeType type) {
			TimeState curSt;
			checkUsage(curSt);
			TimeState dur = (type == FULL) ? 
											diff(tStart_, curSt) : diff(pStart_, curSt);
			if (type == FULL) {
				std::cerr << "---------- " << comment 
						 << " total time usage -----------" << std::endl;
			}
			else {
				std::cerr << "---------- " <<comment 
						 << " period time usage -----------" << std::endl;
			}
			std::cerr << " Real:" << dur.rTime << "s;";
			std::cerr << " User:" << dur.uTime << "s;";
			std::cerr << " System:" << dur.sTime << "s." << std::endl << std::endl;
		}

	private:
		TimeState diff(TimeState &start, TimeState &end) {
			return TimeState(end.rTime - start.rTime, 
											 end.uTime - start.uTime,
											 end.sTime - start.sTime);
		}
		void checkUsage(TimeState &st) const {
			rusage tUsg;
			getrusage(RUSAGE_SELF, &tUsg);
			timeval tReal;
			gettimeofday(&tReal, NULL);
			st.uTime = tUsg.ru_utime.tv_sec + tUsg.ru_utime.tv_usec / TIME_SCALE;
			st.sTime = tUsg.ru_stime.tv_sec + tUsg.ru_stime.tv_usec / TIME_SCALE;
			st.rTime = tReal.tv_sec + tReal.tv_usec / TIME_SCALE;
		}
		TimeState tStart_, pStart_; //total, period
};
// memory
inline double getPeakMemoryUsage() {
#if defined(linux)
  char buf[1000];
  ifstream ifs("/proc/self/stat");
  for(int i = 0; i!= 23; ++i) ifs >> buf;
  return (1.0 / (MEMORY_SCALE * MEMORY_SCALE) * atof(buf)); // GB
#else
  return -1;
#endif
}

#endif