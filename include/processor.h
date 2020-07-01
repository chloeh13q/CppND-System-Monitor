#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>

using std::vector;
using std::string;

class Processor {
 public:
  float Utilization();

 private:
  float pidle, piowait, puser, pnice, psystem, pirq, psoftirq, psteal;
  float idle, iowait, user, nice, system, irq, softirq, steal;
  float prevIdle, Idle, prevNonidle, nonidle, prevTotal, total, totalDiff, idleDiff;
  void UpdatePreviousData();
  void UpdateData(vector<string> data);
};


#endif