#include "transport_system.h"

using namespace std;

TransportSystem::TransportSystem() {
}

void TransportSystem::InsertStop(Stop stop) {
  all_stops_.insert({stop.GetName(), move(stop)});
}

void TransportSystem::InsertBus(Bus bus) {
  all_buses_.insert({bus.GetName(), move(bus)});
}

std::unordered_map<std::string, Stop>& TransportSystem::GetAllStops() {
  return all_stops_;
}

std::unordered_map<std::string, Bus>& TransportSystem::GetAllBuses() {
  return all_buses_;
}

