#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include <iostream>

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() { return Process::pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
  float cpuUti_ = LinuxParser::CpuUtilization(Process::pid_);
  return cpuUti_; 
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Process::pid_); }

// TODO: Return this process's memory utilization
string Process::Ram() { 
  memUti_ = std::stof(LinuxParser::Ram(Process::pid_));
  return LinuxParser::Ram(Process::pid_); 
}

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Process::pid_); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Process::pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return ((this->cpuUti_ - a.cpuUti_)>0) ? true : false;
}

Process::Process(int pid) {
  Process::pid_ = pid;
  Process::cpuUti_ = LinuxParser::CpuUtilization(Process::pid_);
  Process::memUti_ = 0.0;
}