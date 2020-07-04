#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <regex>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) {
    pid_ = pid;
    user_ = Process::User();
    command_ = Process::Command();
    cpu_ = Process::CpuUtilization();
    ram_ = Process::Ram();
    uptime_ = Process::UpTime();
}

// Return this process's ID
int Process::Pid() {
    return pid_;
}

// Return this process's CPU utilization
float Process::CpuUtilization() {
  long uptime = LinuxParser::UpTime();
  long totaltime = LinuxParser::ActiveJiffies(pid_);
  long starttime = LinuxParser::UpTime(pid_);
  float seconds = uptime - starttime / sysconf(_SC_CLK_TCK);
  float value = totaltime / sysconf(_SC_CLK_TCK) / seconds;
  if (value > 0 && value < 1) {
    return 1.0 * value;
  } else{
    return 0.0;
  }
}

// Return the command that generated this process
string Process::Command() {
  string command = LinuxParser::Command(pid_);
  // display only the first 40 characters of a command to avoid overflowing strings
  if (command.length() > 40) {
    command = command.substr(0, 40);
    command += "...";
  }
  return command;
}

// Return this process's memory utilization
string Process::Ram() {
    return LinuxParser::Ram(pid_);
}

// Return the user (name) that generated this process
string Process::User() {
    return LinuxParser::User(pid_);
}

// Return the age of this process (in seconds)
long int Process::UpTime() {
    return LinuxParser::UpTime() - LinuxParser::UpTime(pid_) / sysconf(_SC_CLK_TCK);
  // Per Linux documentation, starttime = the time the process started after system boot
  // Starttime is expressed in clock ticks for Linux 2.6 and after
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
    if (cpu_ < a.cpu_) {
        return true;
    }
    return false;
}