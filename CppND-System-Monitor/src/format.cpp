#include <string>
#include <sstream>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {

  std::ostringstream format;
  int hours = seconds/3600;
  int remainder = seconds%3600;
  int minutes = remainder/60;
  int sSeconds = remainder%60;

  format << hours << ":" << minutes << ":" << sSeconds;
  
  return format.str(); 
}