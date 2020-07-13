#include "buses.h"

Bus::Bus(std::string name) :
    name_(move(name)) {
}

Bus::Bus(std::string name, std::vector<std::string> stops_strings, bool isRing) :
    name_(move(name)), stops_strings_(move(stops_strings)), isRing_(isRing) {
}

std::string Bus::GetName() const {
  return name_;
}

std::vector<std::string> Bus::GetStops() const {
  return stops_strings_;
}

bool Bus::GetIsRing() const {
  return isRing_;
}

bool operator<(const Bus &l, const Bus &r) {
  return l.GetName() == r.GetName();
}

bool operator==(const Bus &l, const Bus &r) {
  return l.GetName() == r.GetName();
}
