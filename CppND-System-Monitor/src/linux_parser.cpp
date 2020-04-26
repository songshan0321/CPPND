#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <cassert>

#include "linux_parser.h"

using std::stof;
using std::stol;
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
  filestream.close();
  return string();
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, temp, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> temp >> kernel;
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

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line;
  string key, value;
  float memTotal = 0.f;
  float memFree = 0.f;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          memTotal = stof(value.c_str());
        }
        if (key == "MemFree") {
          memFree = stof(value.c_str());
        }
        if (memTotal > 0 && memFree > 0) break;
      }
    }
  }
  filestream.close();
  assert(memTotal > memFree);

  return ( memTotal - memFree ) / memTotal; 
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line, upTimeStr, idleTimeStr;
  long upTime, idleTime;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> upTimeStr >> idleTime;
    upTime = stol(upTimeStr.c_str());
  }
  filestream.close();
  return upTime; 
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  
  string user, nice, system, idle, iowait, irq, softirq, steal;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >>
        steal;
  }

  vector<string> v{user,nice,system,idle,iowait,irq,softirq,steal};

  return v;
}

// Overload
float LinuxParser::CpuUtilization(int pid) {
  string line;
  string value;
  float result;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  getline(stream, line);
  string str = line;
  std::istringstream buf(str);
  std::istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);  // done!
  // acquiring relevant times for calculation of active occupation of CPU for
  // selected process
  float utime = UpTime(pid);
  float stime = stof(values[14]);
  // float cutime = stof(values[15]);
  // float cstime = stof(values[16]);
  float starttime = stof(values[21]);
  float uptime = UpTime();  // sys uptime
  float freq = sysconf(_SC_CLK_TCK);
  float total_time = utime + stime;// + cutime + cstime;
  float seconds = uptime - (starttime / freq);
  result = 100.0 * ((total_time / freq) / seconds);
  return result;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string processCountStr, processes, line;
  int processCount = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> processes >> processCountStr;
      if (processes == "processes") {
        processCount = std::stoi(processCountStr);
        break;
      }
    }
  }
  filestream.close();
  return processCount;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string processCountStr, processes, line;
  int processCount = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> processes >> processCountStr;
      if (processes == "procs_running") {
        processCount = std::stoi(processCountStr);
        break;
      }
    }
  }
  filestream.close();
  return processCount;
}


// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string line, cmd = string();
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)){
      cmd += line;
    }
  }
  filestream.close();
  return cmd;
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line, key, value;
  double val;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize") {
          return value;
        }
      }
    }
  }
  filestream.close();
  val = stod(value)/1024;
  return to_string(val);
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string key, value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid") {
          return value;
        }
      }
    }
  }
  filestream.close();

  return value;
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line;
  string key, value1, value2;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value1 >> value2) {
        if (value2.compare(Uid(pid)) == 0) {
          return key;
        }
      }
    }
  }
  filestream.close();

  return key;
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string upTime;
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (size_t i = 0; i < 22; i++) {
      linestream >> upTime;
    }
  }
  return stol(upTime.c_str())/sysconf(_SC_CLK_TCK);
}