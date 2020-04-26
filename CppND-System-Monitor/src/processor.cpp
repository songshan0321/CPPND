#include <string>
#include "linux_parser.h"

#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  std::string line, cpu, user, nice, system, idle, iowait, irq, softirq, steal,
      guest, guest_nice;
  float realTimeUtil = 0;
  std::ifstream filestream(LinuxParser::kProcDirectory +
                           LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> cpu >> user >> nice >> system >> idle >> iowait >>
             irq >> softirq >> steal >> guest >> guest_nice) {
        if (cpu == "cpu") {
          int totalCpuTimeSinceBoot = std::stoi(user) + std::stoi(nice) +
                                      std::stoi(system) + std::stoi(idle) +
                                      std::stoi(iowait) + std::stoi(irq) +
                                      std::stoi(softirq) + std::stoi(steal);

          // Calculate cpu usage since we last checked
          float diff_idle = std::stof(idle.c_str()) - prev_idle_;
          float diff_total = totalCpuTimeSinceBoot - prev_total_;
          realTimeUtil = std::min((diff_total - diff_idle) / diff_total * 2, 100.f);

          // Remember the previous values
          prev_total_ = totalCpuTimeSinceBoot;
          prev_idle_ = std::stoi(idle);

          break;
        }
      }
    }
  }

  return realTimeUtil;
}