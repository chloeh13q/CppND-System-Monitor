#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <regex>
#include <iostream>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) {
    pid_ = pid;
    //user_ = Process::User();
    //command_ = Process::Command();
    // cpu_ = Process::CpuUtilization();
    //ram_ = Process::Ram();
    uptime_ = Process::UpTime();
}

// Return this process's ID
int Process::Pid() {
    return pid_;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return 0; }

// TODO: Return the command that generated this process
string Process::Command() {
    string line;
    std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid_) + LinuxParser::kCmdlineFilename);
    if (stream.is_open()) {
        getline(stream, line);
    }
    return line;
}

// TODO: Return this process's memory utilization
string Process::Ram() {
    string line;
    string s, ram;
    int value;
    std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid_) + LinuxParser::kStatusFilename);
    if (stream.is_open()) {
        getline(stream, line);
        //std::cout << line << std::endl;
        std::istringstream linestream(line);
        linestream >> s;
        while (linestream.peek() != EOF && s != "VmSize:") {
            getline(stream, line);
            std::istringstream linestream(line);
            linestream >> s;
        }
        if (linestream.peek() != EOF) {
            std::istringstream ramstream(line);
            linestream >> s >> ram;
            
            //std::cout << ram << std::endl;
            //value = std::stoi(ram);
            //value /= 1000;
            //ram = to_string(value);
        }
    }
    return ram;
}

// Return the user (name) that generated this process
string Process::User() {
    string line;
    string s;
    string user;
    std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid_) + LinuxParser::kStatusFilename);
    if (stream.is_open()) {
        getline(stream, line);
        std::istringstream linestream(line);
        linestream >> s;
        while (s != "Uid:") {
            getline(stream, line);
            std::istringstream linestream(line);
            linestream >> s;
        }
        std::istringstream userstream(line);
        userstream >> s >> user;
    }
    return user;
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return 0; }

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
    if (cpu_ < a.cpu_) {
        return true;
    }
    return false;
}