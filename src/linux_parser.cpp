#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include "linux_parser.h"

#include <algorithm>

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::map;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
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

long removeKBFromEnd(string s) {
  if(s.substr( s.length() - 2 ) == "kB") {
    return std::stol(s.substr(0, s.length() -2 ));
  } else {
    return std::stol(s);
  }
}

float LinuxParser::MemoryTotal() { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  long memTotal;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      // remove space
      line.erase(std::remove_if(line.begin(), line.end(), isspace),line.end());
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          memTotal = removeKBFromEnd(value);
        }
      }
    }
    return memTotal;
  }
  return 0.0;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  long memTotal;
  // long memFree;
  long memAvailable;
  // long buffers;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      // remove space
      line.erase(std::remove_if(line.begin(), line.end(), isspace),line.end());
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          memTotal = removeKBFromEnd(value);
        }
        if (key == "MemAvailable") {
          memAvailable = removeKBFromEnd(value);
        }
      }
    }
    return ( (memTotal - memAvailable) * 1.0 ) / memTotal;
  }
  return 0.0;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line;
  string uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return std::stol(uptime);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return LinuxParser::IdleJiffies() + LinuxParser::ActiveJiffies();
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
// TODO: Read and return CPU utilization
float LinuxParser::CpuUtilization(int pid) { 
  string line;
  string key;
  long utime, stime, cutime, cstime, starttime;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i=0; i<13; i++) {
      linestream >> key;
    }
    linestream >> utime >> stime >> cutime >> cstime;
    for (int i=18; i<21; i++) {
      linestream >> key;
    }
    linestream >> starttime;

    // https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
    long total_time = utime + stime;
    total_time = total_time + cutime + cstime;
    float Hertz = sysconf(_SC_CLK_TCK);
    long uptime = LinuxParser::UpTime();
    long seconds = uptime - (starttime / Hertz);
    return ((total_time / Hertz) / seconds);
  }
  return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  // user + nice + system + irq + softirq + steal - guest - guest_nice
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  
  string name, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  long total;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> name >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice) {
        if (name=="cpu") {
          total = std::stol(user) + std::stol(nice) + std::stol(system) + std::stol(irq) + std::stol(softirq) + std::stol(steal) - std::stol(guest) - std::stol(guest_nice); 
          break;
        }
      }
    }
    return total;
  }
  return 0;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  // idle + iowait
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  
  string name, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  long total;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> name >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice) {
        if (name=="cpu") {
          total = std::stol(idle) + std::stol(iowait); 
          break;
        }
      }
    }
    return total;
  }
  return 0;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  int totalProcess;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          totalProcess = std::stoi(value);
        }
      }
    }
    return totalProcess;
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  int runningProcess;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          runningProcess = std::stoi(value);
        }
      }
    }
    return runningProcess;
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  string cmd;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }
  return string(); 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  string mem;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        // htop is using RES for memory usage
        if (key == "VmRSS:") {
          mem = value;
          break;
        }
      }
    }
  }
  float ram;
  if (mem.size()>0) {
    ram = std::stol(mem) * 1.0 / 1000 ;
  } else {
    ram = 0;
  }
  return to_string(ram).substr(0, 8); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  string uid;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          uid = value;
        }
      }
    }
    return uid;
  }
  return string(); 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string uid = LinuxParser::Uid(pid);
  string line;
  string key;
  string value;
  std::ifstream filestream(kPasswordPath);
  string name, x, u;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> name >> x >> u) {
        if (u == uid) {
          return name;
        }
      }
    }
  }
  return string(); 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string line;
  string key;
  long value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i=0; i<21; i++) {
      linestream >> key;
    }
    linestream >> value;
    value = value / sysconf(_SC_CLK_TCK);
    return value;
  }
  return 0;
}