#include "ncurses_display.h"
#include "system.h"

#include "linux_parser.h"
#include "process.h"
#include <iostream>

int main() {
  System system;  
  NCursesDisplay::Display(system);
}

// // // dev use
// int main() {
//   // std::cout << LinuxParser::Ram(3264) << std::endl;
//   // std::cout << LinuxParser::Jiffies() << std::endl;
// }