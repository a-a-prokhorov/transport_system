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
    if (auto stop_or_bus = ParseInputRequest(line); holds_alternative<Stop>(stop_or_bus)) {
      transportSystem.InsertStop(get<Stop>(stop_or_bus));
    } else {
      transportSystem.InsertBus(get<Bus>(stop_or_bus));
    }
  }
}

void Processor::process() {
  std::unordered_map<std::string, Stop> &all_stops = transportSystem.GetAllStops();
  std::unordered_map<std::string, Bus> &all_buses = transportSystem.GetAllBuses();

  for (auto& [name, b] : all_buses) {
    vector<string> stops_for_bus = b.GetStops();
    auto stop_it_from_all_stops = all_stops.find(stops_for_bus[0]);
    for (size_t i = 0; i < stops_for_bus.size() - 1; ++i) {
      //add bus to stop
      (*stop_it_from_all_stops).second.buses_through_stop_.insert(name);
      //find next stop for bus
      auto next_stop_it_from_all_stops = all_stops.find(stops_for_bus[i + 1]);
      b.distance_on_earth_ += ((*stop_it_from_all_stops).second).DistanceTo(
          (*next_stop_it_from_all_stops).second);
      stop_it_from_all_stops = next_stop_it_from_all_stops;
    }
    //add bus to last stop
    auto last_stop_it_from_all_stops = all_stops.find(
        stops_for_bus[stops_for_bus.size() - 1]);
    (*last_stop_it_from_all_stops).second.buses_through_stop_.insert(name);

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

void Processor::out(istream &in, ostream &out, int precision) {
  std::unordered_map<std::string, Stop> &all_stops = transportSystem.GetAllStops();
  std::unordered_map<std::string, Bus> &all_buses = transportSystem.GetAllBuses();

  int Q;
  in >> Q;
  string line;
  getline(in, line);
  for (int i = 0; i < Q; ++i) {
    getline(in, line);
    if (auto stop_or_bus = ParseOutputRequest(line); holds_alternative<Stop>(stop_or_bus)) {
      Stop stop_name = get<Stop>(stop_or_bus);
      string name = stop_name.GetName();
      auto it = all_stops.find(name);
      Stop &stop = (*it).second;
      out << "Stop " << name << ":";
      if (it == all_stops.end()) {
        out << " not found" << endl;
      } else if (stop.buses_through_stop_.size() == 0) {
        out << " no buses" << endl;
      } else {
        out << " buses";
        for (string s : stop.buses_through_stop_) {
          out << " " << s;
        }
        out << endl;
      }
    } else {
      Bus bus_name = get<Bus>(stop_or_bus);
      string name = bus_name.GetName();
      auto it = all_buses.find(name);
      Bus &bus = (*it).second;
      out << "Bus " << name << ":";
      if (it == all_buses.end()) {
        out << " not found" << endl;
      } else {
        out << fixed;
        out << " " << bus.stops_on_route_ << " stops on route, " << bus.unique_stops
            << " unique stops, " << setprecision(precision)
            << bus.distance_on_earth_ << " route length" << endl;
      }
    }

  }
}

void RunProcessor() {
  Processor p;
  p.in();
  p.process();
  p.out();
}
