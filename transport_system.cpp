#include "transport_system.h"

using namespace std;

TransportSystem::TransportSystem() {
}

void TransportSystem::InsertStop(Stop &stop) {
  all_stops_.insert(stop);

}

void TransportSystem::InsertBus(Bus &bus) {
  all_buses_.push_back(bus);
}

unordered_set<Stop, StopHasher>& TransportSystem::GetAllStops() {
  return all_stops_;
}

vector<Bus>& TransportSystem::GetAllBuses() {
  return all_buses_;
}

