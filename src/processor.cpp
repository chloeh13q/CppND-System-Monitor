#include <string>
#include <vector>
#include <chrono>
#include <thread>

#include "processor.h"
#include "linux_parser.h"

using std::to_string;
using std::string;
using std::stoi;
using std::vector;

void Processor::UpdatePreviousData() {
    Processor::puser = Processor::user;
    Processor::pnice = Processor::nice;
    Processor::psystem = Processor::system;
    Processor::pidle = Processor::idle;
    Processor::piowait = Processor::iowait;
    Processor::pirq = Processor::irq;
    Processor::psoftirq = Processor::softirq;
    Processor::psteal = Processor::steal;
}

void Processor::UpdateData(vector<string> data) {
    Processor::steal = stof(data.back());
    data.pop_back();
    Processor::softirq = stof(data.back());
    data.pop_back();
    Processor::irq = stof(data.back());
    data.pop_back();
    Processor::iowait = stof(data.back());
    data.pop_back();
    Processor::idle = stof(data.back());
    data.pop_back();
    Processor::system = stof(data.back());
    data.pop_back();
    Processor::nice = stof(data.back());
    data.pop_back();
    Processor::user = stof(data.back());
}


// Return the aggregate CPU utilization
float Processor::Utilization() {
    Processor::UpdatePreviousData();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    vector<string> data = LinuxParser::CpuUtilization();
    Processor::UpdateData(data);
    float cpu;
    prevIdle = pidle + piowait;
    Idle = idle + iowait;
    prevNonidle = puser + pnice + psystem + pirq + psoftirq + psteal;
    nonidle = user + nice + system + irq + softirq + steal;
    prevTotal = prevIdle + prevNonidle;
    total = Idle + nonidle;
    totalDiff = total - prevTotal;
    idleDiff = Idle - prevIdle;
    cpu = (totalDiff - idleDiff) / (totalDiff + 0.0001);
    return cpu;
}
