#pragma once

#include "transport_system.h"
#include "json.h"

#include <iostream>

class Processor {
public:
  void in(std::istream &in = std::cin);

  void inJson(std::istream &in = std::cin);

  void process();

  void out(std::istream &in = std::cin, std::ostream &out = std::cout, int precision = 6);

  void outJson(std::ostream &out = std::cout, int precision = 6);

  std::map<std::string, Json::Node> requests;

private:
  TransportSystem transportSystem;
};

void RunProcessor(bool isJson = false);
