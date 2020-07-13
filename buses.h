#pragma once

#include <vector>
#include <set>

class Bus {
public:
  Bus(std::string name);
  Bus(std::string name,
      std::vector<std::string> stops_strings,
      bool isRing);

  std::string GetName() const;

  std::vector<std::string> GetStops() const;

  bool GetIsRing() const;

  int stops_on_route_ = 0;
  int unique_stops = 0;
  double distance_on_earth_ = 0.0;
  double real_distance_ = 0.0;

private:
  std::string name_;
  std::vector<std::string> stops_strings_;
  bool isRing_ = false;

};

bool operator< (const Bus& l, const Bus& r);

bool operator== (const Bus& l, const Bus& r);
