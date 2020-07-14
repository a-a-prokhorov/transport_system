#pragma once

#include "stops.h"
#include "buses.h"

//#include <unordered_set>
#include <unordered_map>

class TransportSystem {
public:
  TransportSystem();

  void InsertStop(Stop stop);

  void InsertBus(Bus bus);

  std::unordered_map<std::string, Stop>& GetAllStops();

  std::unordered_map<std::string, Bus>& GetAllBuses();

private:
  std::unordered_map<std::string, Stop> all_stops_;
  std::unordered_map<std::string, Bus> all_buses_;
};
