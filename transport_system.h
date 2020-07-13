#pragma once

#include "stops.h"
#include "buses.h"

#include <unordered_set>

class TransportSystem {
public:
  TransportSystem();

  void InsertStop(Stop &stop);

  void InsertBus(Bus &bus);

  std::unordered_set<Stop, StopHasher>& GetAllStops();

  std::vector<Bus>& GetAllBuses();

private:
  std::unordered_set<Stop, StopHasher> all_stops_;
  std::vector<Bus> all_buses_;
};
