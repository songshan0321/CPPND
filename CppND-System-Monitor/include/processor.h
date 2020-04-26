#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp
  long prev_idle_;
  long prev_total_;
  // TODO: Declare any necessary private members
 private:
};

#endif