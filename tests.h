#pragma once

#include "test_runner.h"
#include "parse_requests.h"
#include <variant>

void TestParseInputRequest() {
  {
    std::string test_input_stop = "Stop Tol: 55.611087, 37.20829";
    Stop stop = std::get<Stop>(ParseInputRequest(test_input_stop));
    double lat = 55.611087;
    double lon = 37.20829;
    Stop stop_expected = Stop("Tol", lat, lon);
    ASSERT_EQUAL(stop.GetName(), stop_expected.GetName());
    ASSERT_EQUAL(stop.GetLat(), stop_expected.GetLat());
    ASSERT_EQUAL(stop.GetLon(), stop_expected.GetLon());
  }

  {
    std::string test_input_bus = "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka";
    Bus bus = std::get<Bus>(ParseInputRequest(test_input_bus));
    Bus bus_expected = Bus("750", {"Tolstopaltsevo", "Marushkino", "Rasskazovka"}, false);
    ASSERT_EQUAL(bus.GetName(), bus_expected.GetName());
    ASSERT_EQUAL(bus.GetIsRing(), bus_expected.GetIsRing());
    std::vector<std::string> bus_expected_stops = bus_expected.GetStops();
    std::vector<std::string> bus_stops = bus.GetStops();
    for (size_t i = 0; i < bus_expected_stops.size(); ++i) {
      ASSERT_EQUAL(bus_stops[i], bus_expected_stops[i]);
    }
  }

  {
    std:: string test_input_bus = "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye";
    Bus bus = std::get<Bus>(ParseInputRequest(test_input_bus));
    Bus bus_expected = Bus("256", {"Biryulyovo Zapadnoye", "Biryusinka", "Universam", "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya", "Biryulyovo Zapadnoye"},  true);
    ASSERT_EQUAL(bus.GetName(), bus_expected.GetName());
    ASSERT_EQUAL(bus.GetIsRing(), bus_expected.GetIsRing());
    std::vector<std::string> bus_expected_stops = bus_expected.GetStops();
    std::vector<std::string> bus_stops = bus.GetStops();
    for (size_t i = 0; i < bus_expected_stops.size(); ++i) {
      ASSERT_EQUAL(bus_stops[i], bus_expected_stops[i]);
    }
  }
}

void TestAll() {
  TestRunner tr;
  RUN_TEST(tr, TestParseInputRequest);
}
