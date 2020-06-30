#include <string>
#include <fstream>
#include <regex>
#include <chrono>
#include <thread>

#include "processor.h"

using std::to_string;
using std::string;
using std::stoi;

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

void Processor::UpdateData(int data[8]) {
    Processor::user = data[0];
    Processor::nice = data[1];
    Processor::system = data[2];
    Processor::idle = data[3];
    Processor::iowait = data[4];
    Processor::irq = data[5];
    Processor::softirq = data[6];
    Processor::steal = data[7];
}

void Processor::UtilizationHelper() {
    string line;
    string value[8];
    string s;
    int data[8];
    UpdatePreviousData();
    std::ifstream stream("/proc/stat");
    if (stream.is_open()) {
        getline(stream, line);
        std::istringstream linestream(line);
        linestream >> s;
        for (int i = 0; i < 8; i++) {
            linestream >> value[i];
            data[i] = stoi(value[i]);
        }
        UpdateData(data);
    }
}

// Return the aggregate CPU utilization
float Processor::Utilization() {
    Processor::UtilizationHelper();
    float cpu;
    prevIdle = pidle + piowait;
    Idle = idle + iowait;
    prevNonidle = puser + pnice + psystem + pirq + psoftirq + psteal;
    nonidle = user + nice + system + irq + softirq + steal;
    prevTotal = prevIdle + prevNonidle;
    total = Idle + nonidle;
    totalDiff = total - prevTotal;
    idleDiff = Idle - prevIdle;
    cpu = (totalDiff - idleDiff) / totalDiff;
    return cpu;
}
