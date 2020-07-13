#include "processor.h"
#include "parse_requests.h"

#include <algorithm>
#include <iomanip>

using namespace std;

void Processor::in(istream &in) {
  int Q;
  in >> Q;
  string line;
  getline(in, line);
  for (int i = 0; i < Q; ++i) {
    getline(in, line);
    if (auto stop_or_bus = ParseInputRequest(line); holds_alternative<Stop>(
        stop_or_bus)) {
      transportSystem.InsertStop(get<Stop>(stop_or_bus));
    } else {
      transportSystem.InsertBus(get<Bus>(stop_or_bus));
    }
  }
}

void Processor::process() {
  unordered_set<Stop, StopHasher> &all_stops = transportSystem.GetAllStops();
  vector<Bus> &all_buses = transportSystem.GetAllBuses();

  for (Bus &b : all_buses) {
    vector<string> stops_for_bus = b.GetStops();
    auto stop_it = all_stops.find(Stop(stops_for_bus[0]));
    for (size_t i = 0; i < stops_for_bus.size() - 1; ++i) {
      auto next_stop_it = all_stops.find(Stop(stops_for_bus[i + 1]));
      b.distance_on_earth_ += (*stop_it).DistanceTo(*next_stop_it);
      stop_it = next_stop_it;
    }
    set<string> stops_for_bus_unique(stops_for_bus.begin(),
        stops_for_bus.end());
    b.unique_stops = stops_for_bus_unique.size();
    if (b.GetIsRing()) {
      b.stops_on_route_ = stops_for_bus.size();
    } else {
      b.stops_on_route_ = 2 * stops_for_bus.size() - 1;
      b.distance_on_earth_ *= 2;
    }
  }
}

void Processor::out(istream &in, ostream &out) {
  vector<Bus> &all_buses = transportSystem.GetAllBuses();
  int Q;
  in >> Q;
  string line;
  getline(in, line);
  for (int i = 0; i < Q; ++i) {
    getline(in, line);
    string name = ParseOutputRequest(line);
    auto it = find(all_buses.begin(), all_buses.end(), Bus(name));
    out << "Bus " << name << ": ";
    if (it == all_buses.end()) {
      out << "not found" << endl;
    } else {
      out << fixed;
      out << (*it).stops_on_route_ << " stops on route, " << (*it).unique_stops
          << " unique stops, " << setprecision(6) << (*it).distance_on_earth_
          << " route length" << endl;
    }
  }
}

void RunProcessor() {
  Processor p;
  p.in();
  p.process();
  p.out();
}
