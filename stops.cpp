#include "stops.h"

using namespace std;

double DegreeToRad(double deg) {
  return deg * PI / 180.0;
}

Stop::Stop(string name) :
    name_(move(name)) {
}

Stop::Stop(string name, double lat, double lon, unordered_map<string, int> distances_to_other_stops) :
    name_(move(name)), lat_(lat), lon_(lon), distances_to_other_stops_(distances_to_other_stops) {
}

string Stop::GetName() const {
  return name_;
}

double Stop::GetLat() const {
  return lat_;
}

double Stop::GetLon() const {
  return lon_;
}

double Stop::DistanceTo(const Stop &other) const {
  double abs_lon_diff = abs(
      DegreeToRad(lon_) - DegreeToRad(other.GetLon()));
  double lat_rad = DegreeToRad(lat_);
  double other_lat_rad = DegreeToRad(other.GetLat());
  return acos(
      sin(lat_rad) * sin(other_lat_rad)
          + cos(lat_rad) * cos(other_lat_rad)
              * cos(abs_lon_diff)) * EARTH_RADIUS;
}
