#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
  int HH = seconds / 3600;
  int MM = seconds % 3600 / 60;
  int SS = seconds % 3600 % 60;

  string HH_str = std::to_string(HH);
  string MM_str = std::to_string(MM);
  string SS_str = std::to_string(SS);

  if (HH < 10) {
    HH_str = "0" + HH_str;
  }
  if (MM < 10) {
    MM_str = "0" + MM_str;
  }
  if (SS < 10) {
    SS_str = "0" + SS_str;
  }
  return HH_str + ":" + MM_str + ":" + SS_str;
 }