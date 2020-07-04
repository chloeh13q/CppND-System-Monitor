#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

template <typename T>
T FindValueByKey(std::string const &KeyFilter, std::string const &FilePath) {
  std::string line;
  std::string key;
  T value;
  std::ifstream stream(FilePath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == KeyFilter) {
          return value;
        }
      }
    }
    stream.close();
  }
  return value;
};

template <typename T>
T GetFirstValueFromFile(std::string const &FilePath) {
  std::string line;
  T value;
  std::ifstream stream(FilePath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> value) {
        return value;
      }
    }
    stream.close();
  }
  return value;
};

// Read and return system operating system info
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value = "N/A";
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (getline(filestream, line)) {
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
    filestream.close();
  }
  return value;
}

// Read and return system kernel info
string LinuxParser::Kernel() {
  string os, version, kernel = "N/A";
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
    stream.close();
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
  float total = FindValueByKey<float>(filterMemTotalString, kProcDirectory + kMeminfoFilename);
  float free = FindValueByKey<float>(filterMemFreeString, kProcDirectory + kMeminfoFilename);
  return (total - free) / total;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  long output = GetFirstValueFromFile<long>(kProcDirectory + kUptimeFilename);
  return output;
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
  long total = 0;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 13; i++) {
      linestream >> s;
    }
    for (int j = 0; j < 4; j++) {
      linestream >> s;
      value[j] = s;
      jiffies[j] = std::stol(value[j]);
    }
    stream.close();
  }
  total = jiffies[0] + jiffies[1] + jiffies[2] + jiffies[3];
  return total;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  string line;
  string s;
  string value;
  long jiffies = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::istringstream linestream(line);
    linestream >> s;
    for (int i = 0; i < 10; i++) {
      linestream >> value;
      if (!value.empty()) {
        jiffies += std::stol(value);
      }
    }
    stream.close();
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
    stream.close();
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
          value.emplace_back(data[i]);
      }
    stream.close();
  }
  return value;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int value = FindValueByKey<int>(filterProcesses, kProcDirectory + kStatFilename);
  return value;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int value = FindValueByKey<int>(filterRunningProcesses, kProcDirectory + kStatFilename);
  return value;
}

//  Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string command = GetFirstValueFromFile<string>(kProcDirectory + to_string(pid) + kCmdlineFilename);
  return command;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  // using VmData instead of VmSize here to get the exact physical memory being used as a part of Physical RAM
  int value = FindValueByKey<int>(filterProcMem, kProcDirectory + to_string(pid) + kStatusFilename);
  return to_string(value / 1000);
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string value = FindValueByKey<string>(filterUID, kProcDirectory + to_string(pid) + kStatusFilename);
  return value;
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
    stream.close();
  }
  return "N/A";
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line;
  string value;
  long uptime = 0;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; i++) {
      linestream >> value;
    }
    uptime = std::stol(value);
    stream.close();
  }
  return uptime;
}