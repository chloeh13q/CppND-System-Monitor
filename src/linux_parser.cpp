#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// Read and return system operating system info
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return system kernel info
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// Will maybe update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string mem, value;
  float memory[5];
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    for (int i = 0; i < 5; i++) {
      getline(stream, line);
      std::istringstream linestream(line);
      linestream >> mem >> value;
      memory[i] = stof(value);
    }
  }
  return (memory[0] - memory[1]) / memory[0];
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  string value;
  long uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
    uptime = std::stol(value);
  }
  return uptime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::UpTime() * sysconf(_SC_CLK_TCK);
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string s;
  string value[4];
  long jiffies[4];
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 13; i++) {
      linestream >> s;
    }
    for (int j = 13; j < 17; j++) {
      linestream >> s;
      value[j - 13] = s;
      jiffies[j - 13] = std::stol(value[j - 13]);
    }
  }
  long total = jiffies[0] + jiffies[1] + jiffies[2] + jiffies[3];
  return total;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  string line;
  string s, value;
  long jiffies = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::istringstream linestream(line);
    linestream >> s;
    for (int i = 0; i < 10; i++) {
      linestream >> value;
      jiffies += std::stol(value);
    }
  }
  return jiffies;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  string line;
  string s, value;
  long jiffies = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::istringstream linestream(line);
    linestream >> s;
    for (int i = 0; i < 4; i++) {
      linestream >> value;
    }
    jiffies += std::stol(value);
    linestream >> value;
    jiffies += std::stol(value);
  }
  return jiffies;
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  vector<string> value;
  string data[8];
  string s;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
      getline(stream, line);
      std::istringstream linestream(line);
      linestream >> s;
      for (int i = 0; i < 8; i++) {
          linestream >> data[i];
          value.push_back(data[i]);
      }
  }
  return value;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string s, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> s >> value;
      if (s == "processes") {
        return std::stoi(value);
      }
    }
  }
  return 0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string s, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> s >> value;
      if (s == "procs_running") {
      return std::stoi(value);
      }
    }
  }
  return 0;
}

//  Read and return the command associated with a process
string LinuxParser::Command(int pid) {
    string line;
    std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kCmdlineFilename);
    if (stream.is_open()) {
        getline(stream, line);
    }
    return line;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string s, ram;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
      while (std::getline(stream, line)) {
        std::istringstream linestream(line);
        linestream >> s >> ram;
        if (s == "VmSize:") {
          return to_string(std::stol(ram) / 1000);
        }
      }
    } else {
      ram = "0.0";
    }
  return ram;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string s;
  string uid;
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatusFilename);
  if (stream.is_open()) {
      while (getline(stream, line)) {
        std::istringstream linestream(line);
        linestream >> s >> uid;
        if (s == "Uid:") {
          return uid;
        }
      }
    }
  return "None";
  }

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);
  string line;
  string user, x, p;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x >> p;
      if (p == uid) {
        return user;
      }
    }
  }
  return "None";
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line;
  string value;
  long uptime;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; i++) {
      linestream >> value;
    }
    uptime = std::stol(value);
  }
  return uptime;
}