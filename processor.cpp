#include "processor.h"
#include "parse_requests.h"

#include <algorithm>
#include <iomanip>
#include <stdexcept>

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

void Processor::inJson(istream &in) {
  requests = Json::Load(in).GetRoot().AsMap();
  vector<Json::Node> base_requests = requests["base_requests"].AsArray();
  map<string, Json::Node> req_map;
  string req_type, stop_name, bus_name;
  double lat, lon;
  bool isRing;
  for (Json::Node& req : base_requests) {
    req_map = req.AsMap();
    req_type = req_map["type"].AsString();
    if (req_type == "Stop") {
      unordered_map<string, int> stops_for_stop;
      stop_name = req_map["name"].AsString();
      lat = req_map["latitude"].AsDouble();
      lon = req_map["longitude"].AsDouble();
      map<string, Json::Node> stops_node = req_map["road_distances"].AsMap();
      for (auto &[stop_name, distance] : stops_node) {
        stops_for_stop[stop_name] = distance.AsInt();
      }
      Stop s = Stop(stop_name, lat, lon, stops_for_stop);
      transportSystem.InsertStop(s);
    } else if(req_type == "Bus") {
      vector<string> stops_for_bus;
      bus_name = req_map["name"].AsString();
      isRing = req_map["is_roundtrip"].AsBool();
      vector<Json::Node> stops_node = req_map["stops"].AsArray();
      for (auto &n : stops_node) {
        stops_for_bus.push_back(n.AsString());
      }
      Bus b = Bus(bus_name, stops_for_bus, static_cast<bool>(isRing));
      transportSystem.InsertBus(b);
    } else {
      throw runtime_error("unknown request");
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
      if(name_of_other_stop != name) {
        all_real_distances[name][name_of_other_stop] = distance;
        if ((*all_stops.find(name_of_other_stop)).second.distances_to_other_stops_.count(name) == 0) {
          all_real_distances[name_of_other_stop][name] = distance;
        }
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

void Processor::outJson(std::ostream &out, int precision) {
  std::unordered_map<std::string, Stop> &all_stops = transportSystem.GetAllStops();
  std::unordered_map<std::string, Bus> &all_buses = transportSystem.GetAllBuses();

  vector<Json::Node> stat_requests = requests["stat_requests"].AsArray();
  map<string, Json::Node> req_map;
  string req_type, name;
  int req_id;
  out << fixed;
  out << "[\n";
  int i = stat_requests.size();
  for (Json::Node& req : stat_requests) {
    --i;
    req_map = req.AsMap();
    req_type = req_map["type"].AsString();
    req_id = req_map["id"].AsInt();
    name = req_map["name"].AsString();
    if (req_type == "Stop") {
      auto it = all_stops.find(name);
      Stop &stop = (*it).second;
      out << "{\n";
      if (it == all_stops.end()) {
        out << "\"request_id\": " << req_id  << ",\n"
            << "\"error_message\": " << "\"not found\"" << "\n";
      } else if (stop.buses_through_stop_.size() == 0) {
        out << "\"buses\": []," << "\n"
            << "\"request_id\": " << req_id  << "\n";
      } else {
        out << "\"buses\": [\n";
        int j = stop.buses_through_stop_.size();
        for (string s : stop.buses_through_stop_) {
          --j;
          out << "\"" << s << "\"";
          if (j != 0) {
            out << ",";
          }
          out << "\n";
        }
        out << "],\n"
            << "\"request_id\": " << req_id  << "\n";
      }
      out << "}";
      if (i != 0) {
        out << ",";
      }
      out << "\n";
    } else if (req_type == "Bus") {
      auto it = all_buses.find(name);
      Bus &bus = (*it).second;
      out << "{\n";
      if (it == all_buses.end()) {
        out << "\"request_id\": " << req_id  << ",\n"
            << "\"error_message\": " << "\"not found\"" << "\n";
      } else {
      out << "\"route_length\": " << bus.real_distance_ << ",\n"
          << "\"request_id\": " << req_id  << ",\n"
          << "\"curvature\": " << setprecision(precision) << bus.curvature_ << ",\n"
          << "\"stop_count\": " << bus.stops_on_route_ << ",\n"
          << "\"unique_stop_count\": " << bus.unique_stops << "\n";
      }
      out << "}";
      if (i != 0) {
        out << ",";
      }
      out << "\n";
    } else {
      throw runtime_error("unknown request");
    }
  }
  out << "]\n";
}

void RunProcessor(bool isJson) {
  Processor p;
  if (isJson) {
    p.inJson();
  } else {
    p.in();
  }
  p.process();
  if (isJson) {
    p.outJson();
  }
  else {
    p.out();
  }
}
