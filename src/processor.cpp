#include "processor.h"
#include "linux_parser.h"
#include <string>
#include <vector>
#include "process.h"

using std::string;
using std::vector;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  return Processor::TotalCpuUtilization();
}

// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
float Processor::TotalCpuUtilization() { 
  long preJ = preJiffes[0];
  long preI = preJiffes[1];
  long totald = LinuxParser::Jiffies() - preJ;
  long idled = LinuxParser::IdleJiffies() - preI;

  // update now to prev for next time calculate
  preJiffes[0] = LinuxParser::Jiffies();
  preJiffes[1] = LinuxParser::IdleJiffies();
  
  return (totald - idled) * 1.0 / totald;
}