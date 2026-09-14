#pragma once

#include <iostream>

// Prints out a TODO message at most once.
#define TODO                                                                  \
  {                                                                           \
    static std::ostream& todo = std::cout << "TODO: " << __FUNCTION__         \
                                          << " in " << __FILE__ << std::endl; \
    (void)todo;                                                               \
  }
