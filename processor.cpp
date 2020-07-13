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
  unordered_map<string, Stop> &all_stops = transportSystem.GetAllStops();
  unordered_map<string, Bus> &all_buses = transportSystem.GetAllBuses();

  //calculate real distances
  unordered_map<string, unordered_map<string, int>> all_real_distances;
  for (auto& [name, stop] : all_stops) {
    for (auto& [name_of_other_stop, distance] : stop.distances_to_other_stops_) {
      all_real_distances[name][name_of_other_stop] = distance;
      if ((*all_stops.find(name_of_other_stop)).second.distances_to_other_stops_.count(name) == 0) {
        all_real_distances[name_of_other_stop][name] = distance;
      }
    }
  }

  //calculate bus info
  for (auto& [name, bus] : all_buses) {
    vector<string> stops_for_bus = bus.GetStops();
    auto stop_it_from_all_stops = all_stops.find(stops_for_bus[0]);
    for (size_t i = 0; i < stops_for_bus.size() - 1; ++i) {
      //add bus to stop
      (*stop_it_from_all_stops).second.buses_through_stop_.insert(name);
      //find next stop for bus
      auto next_stop_it_from_all_stops = all_stops.find(stops_for_bus[i + 1]);
      bus.distance_on_earth_ += ((*stop_it_from_all_stops).second).DistanceTo((*next_stop_it_from_all_stops).second);
      bus.real_distance_ += all_real_distances[(*stop_it_from_all_stops).second.GetName()][(*next_stop_it_from_all_stops).second.GetName()];
      stop_it_from_all_stops = next_stop_it_from_all_stops;
    }
    //add bus to last stop
    auto last_stop_it_from_all_stops = all_stops.find(stops_for_bus[stops_for_bus.size() - 1]);
    (*last_stop_it_from_all_stops).second.buses_through_stop_.insert(name);

    //unique stops for bus
    set<string> stops_for_bus_unique(stops_for_bus.begin(), stops_for_bus.end());
    bus.unique_stops = stops_for_bus_unique.size();

    //there are some differences between straight and ring path
    if (bus.GetIsRing()) {
      bus.stops_on_route_ = stops_for_bus.size();
    } else {
      bus.stops_on_route_ = 2 * stops_for_bus.size() - 1;
      bus.distance_on_earth_ *= 2;
      for (size_t i = stops_for_bus.size() - 1; i > 0; --i) {
        auto stop_it_from_all_stops = all_stops.find(stops_for_bus[i]);
        auto next_stop_it_from_all_stops = all_stops.find(stops_for_bus[i - 1]);
        bus.real_distance_ += all_real_distances[(*stop_it_from_all_stops).second.GetName()][(*next_stop_it_from_all_stops).second.GetName()];
      }
    }
    bus.curvature_ = bus.real_distance_ / bus.distance_on_earth_;
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
            << bus.real_distance_ << " route length, "
            << bus.curvature_ << " curvature" << endl;
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
