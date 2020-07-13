#pragma once

#include <map>
#include <string>
#include <set>
#include <cmath>

const double PI = 3.1415926535;
const double EARTH_RADIUS = 6'371'000.0;

double DegreeToRad(double deg);

class Stop {
public:
  Stop(std::string name);
  Stop(std::string name, double lat, double lon);

  std::string GetName() const;

  double GetLat() const;

  double GetLon() const;

  double DistanceTo(const Stop &other) const;

private:
  const std::string name_;
  double lat_ = 0.0;
  double lon_ = 0.0;
};

struct StopHasher {
  size_t operator()(const Stop &s) const;
};

bool operator==(const Stop &l, const Stop &r);
