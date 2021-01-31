#include <string>

#include "format.h"

using std::string;
using std::to_string;

string addZoreFront(int number) {
  if (number<10) {
    return '0' + to_string(number);
  } else {
    return to_string(number);
  }
}

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(const long seconds) {
  int hour = seconds / (60 * 60);
  int mintue = ( seconds % (60 * 60) ) / 60;
  int second = seconds % 60;

  return addZoreFront(hour) + ":" + addZoreFront(mintue) + ":" + addZoreFront(second);
}