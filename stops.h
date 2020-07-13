#pragma once

#include <string>
#include <set>
#include <unordered_map>
#include <cmath>

const double PI = 3.1415926535;
const double EARTH_RADIUS = 6'371'000.0;

double DegreeToRad(double deg);

class Stop {
private:
  const std::string name_;
  double lat_ = 0.0;
  double lon_ = 0.0;

public:
  Stop(std::string name);

  Stop(std::string name, double lat, double lon, std::unordered_map<std::string, int> distances_to_other_stops = {});

  std::string GetName() const;

  double GetLat() const;

  double GetLon() const;

  double DistanceTo(const Stop &other) const;

  std::set<std::string> buses_through_stop_;

  std::unordered_map<std::string, int> distances_to_other_stops_;
};
