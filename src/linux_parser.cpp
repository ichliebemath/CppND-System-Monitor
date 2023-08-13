#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line;
  string key;
  string value;
  string kB;
  float MemTotal, MemFree;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(stream.is_open()) {
    while(getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> key >> value >> kB) {
        if (key == "MemTotal") {
            MemTotal = stof(value);
          } else if (key == "MemFree") {
            MemFree = stof(value);
          }
      }
    }
  }
  return ((MemTotal - MemFree) / MemTotal);
}

long LinuxParser::UpTime() {  
  string line;
  string uptime;
  string idletime;
  long valid_uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idletime;
      valid_uptime = std::stol(uptime);
    } 
  return valid_uptime;
}

long LinuxParser::Jiffies() {
  auto jiffies = LinuxParser::CpuUtilization();
  return stol(jiffies[CPUStates::kUser_]) + stol(jiffies[CPUStates::kNice_]) + 
         stol(jiffies[CPUStates::kSystem_]) + stol(jiffies[kIRQ_]) +
         stol(jiffies[kSoftIRQ_]) + stol(jiffies[kSteal_]) +
         stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]);
} 


long LinuxParser::ActiveJiffies(int pid) {
    string line, value;
    vector<string> jiffies;
    std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
    if(stream.is_open()) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      while(linestream >> value) {
        jiffies.emplace_back(value);
      }
    }
    return stol(jiffies[13]) + stol(jiffies[14]) + stol(jiffies[15]) + stol(jiffies[16]);
}

long LinuxParser::ActiveJiffies() {
  auto jiffies = LinuxParser::CpuUtilization();
  return stol(jiffies[CPUStates::kUser_]) + stol(jiffies[CPUStates::kNice_]) + 
         stol(jiffies[CPUStates::kSystem_]) + stol(jiffies[kIRQ_]) +
         stol(jiffies[kSoftIRQ_]) + stol(jiffies[kSteal_]);
 }

long LinuxParser::IdleJiffies() { 
  auto jiffies = LinuxParser::CpuUtilization();;
  return stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]);
}

vector<string> LinuxParser::CpuUtilization() { 
  string line, cpu, value;
  vector<string> jiffies;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    while(linestream >> value) {
      jiffies.emplace_back(value);
    }    
  }
  return jiffies;
}

int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        return std::stoi(value);
      }
    }
  }
  return std::stoi(value);
}

int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
        if (key == "procs_running") {
          return std::stoi(value);
        }
      }
    }
  }
   return std::stoi(value);
}

string LinuxParser::Command(int pid) { 
  string line, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if(stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return value;
}

string LinuxParser::Ram(int pid) { 
  string line, key, value;
  int valid_value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()){
    while(std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value;
      // VmSize replaced with VmRss as VmRss gives the exact physical memory used as a part of RAM
      if(key == "VmRss"){
        valid_value = std::stoi(value);
      }
    }
  }
  return to_string(valid_value / 1024);
}

string LinuxParser::Uid(int pid) {
  string line, key, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(stream.is_open()){
    while(getline(stream, line)) {
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream linestream(line);
    linestream >> key >> value;
    if (key == "Uid") {
      return value;
      }
    }
  stream.close();
  }
  return "";
 }


string LinuxParser::User(int pid) {
  string line;
  string username, str1, str2;
  string uid = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream linestream(line);
      while (linestream >> username >> str1 >> str2) {
        if (str2 == uid) {
          return username;
        }
      }
    }
  }
  return username;
}

long LinuxParser::UpTime(int pid) {
  string line, value;
  vector<string> jiffies;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if(stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> value) {
      jiffies.emplace_back(value);
    }
  }
  return LinuxParser::UpTime() - (stol(jiffies[21]) / sysconf(_SC_CLK_TCK));
}
