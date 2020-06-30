#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
  int pidle, piowait, puser, pnice, psystem, pirq, psoftirq, psteal;
  int idle, iowait, user, nice, system, irq, softirq, steal;
  int prevIdle, Idle, prevNonidle, nonidle, prevTotal, total, totalDiff, idleDiff;
  void UpdatePreviousData();
  void UpdateData(int data[8]);
  void UtilizationHelper();
};


#endif