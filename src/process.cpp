#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include "process.h"
#include "linux_parser.h"
using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() { 
  return Pid_;
 }

// Return this process's CPU utilization
float Process::CpuUtilization() { 
  long total_time = LinuxParser::ActiveJiffies(Pid_);
  long seconds = LinuxParser::UpTime(Pid_);
  float cpu_usage = float(total_time) / float(seconds);
  return cpu_usage;
 }

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Pid_); }

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Pid_); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid_); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
  bool test;
  test = this->CpuUtilization() > a.CpuUtilization();
  return test;
}