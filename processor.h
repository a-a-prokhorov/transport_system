#pragma once

#include "transport_system.h"

#include <iostream>

class Processor {
public:
  void in(std::istream &in = std::cin);

  void process();

  void out(std::istream &in = std::cin, std::ostream &out = std::cout);

private:
  TransportSystem transportSystem;
};

void RunProcessor();
