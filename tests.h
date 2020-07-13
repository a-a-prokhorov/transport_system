#pragma once

#include "test_runner.h"
#include "parse_requests.h"
#include "processor.h"

#include <variant>
#include <sstream>

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
    std::string test_input_bus =
        "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka";
    Bus bus = std::get<Bus>(ParseInputRequest(test_input_bus));
    Bus bus_expected = Bus("750", { "Tolstopaltsevo", "Marushkino",
        "Rasskazovka" }, false);
    ASSERT_EQUAL(bus.GetName(), bus_expected.GetName());
    ASSERT_EQUAL(bus.GetIsRing(), bus_expected.GetIsRing());
    std::vector<std::string> bus_expected_stops = bus_expected.GetStops();
    std::vector<std::string> bus_stops = bus.GetStops();
    for (size_t i = 0; i < bus_expected_stops.size(); ++i) {
      ASSERT_EQUAL(bus_stops[i], bus_expected_stops[i]);
    }
  }

  {
    std::string test_input_bus =
        "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye";
    Bus bus = std::get<Bus>(ParseInputRequest(test_input_bus));
    Bus bus_expected = Bus("256", { "Biryulyovo Zapadnoye", "Biryusinka",
        "Universam", "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya",
        "Biryulyovo Zapadnoye" }, true);
    ASSERT_EQUAL(bus.GetName(), bus_expected.GetName());
    ASSERT_EQUAL(bus.GetIsRing(), bus_expected.GetIsRing());
    std::vector<std::string> bus_expected_stops = bus_expected.GetStops();
    std::vector<std::string> bus_stops = bus.GetStops();
    for (size_t i = 0; i < bus_expected_stops.size(); ++i) {
      ASSERT_EQUAL(bus_stops[i], bus_expected_stops[i]);
    }
  }
}

void TestRunProcessor_A() {
  Processor p;
  std::string s = std::string("10\n");
  s.append("Stop Tolstopaltsevo: 55.611087, 37.20829\n");
  s.append("Stop Marushkino: 55.595884, 37.209755\n");
  s.append("Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n");
  s.append("Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka\n");
  s.append("Stop Rasskazovka: 55.632761, 37.333324\n");
  s.append("Stop Biryulyovo Zapadnoye: 55.574371, 37.6517\n");
  s.append("Stop Biryusinka: 55.581065, 37.64839\n");
  s.append("Stop Universam: 55.587655, 37.645687\n");
  s.append("Stop Biryulyovo Tovarnaya: 55.592028, 37.653656\n");
  s.append("Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164\n");
  s.append("3\n");
  s.append("Bus 256\n");
  s.append("Bus 750\n");
  s.append("Bus 751\n");

  std::istringstream in_stream(s);
  std::ostringstream out_stream;
  p.in(in_stream);
  p.process();
  p.out(in_stream, out_stream, 2);

  std::string output = out_stream.str();
  std::string expected_output = std::string("Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length\n");
  expected_output.append("Bus 750: 5 stops on route, 3 unique stops, 20939.48 route length\n");
  expected_output.append("Bus 751: not found\n");
  ASSERT_EQUAL(output, expected_output);
}

void TestRunProcessor_B() {
  Processor p;
  std::string s = std::string("13\n");
  s.append("Stop Tolstopaltsevo: 55.611087, 37.20829\n");
  s.append("Stop Marushkino: 55.595884, 37.209755\n");
  s.append("Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n");
  s.append("Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka\n");
  s.append("Stop Rasskazovka: 55.632761, 37.333324\n");
  s.append("Stop Biryulyovo Zapadnoye: 55.574371, 37.6517\n");
  s.append("Stop Biryusinka: 55.581065, 37.64839\n");
  s.append("Stop Universam: 55.587655, 37.645687\n");
  s.append("Stop Biryulyovo Tovarnaya: 55.592028, 37.653656\n");
  s.append("Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164\n");
  s.append("Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye\n");
  s.append("Stop Rossoshanskaya ulitsa: 55.595579, 37.605757\n");
  s.append("Stop Prazhskaya: 55.611678, 37.603831\n");
  s.append("6\n");
  s.append("Bus 256\n");
  s.append("Bus 750\n");
  s.append("Bus 751\n");
  s.append("Stop Samara\n");
  s.append("Stop Prazhskaya\n");
  s.append("Stop Biryulyovo Zapadnoye\n");

  std::istringstream in_stream(s);
  std::ostringstream out_stream;
  p.in(in_stream);
  p.process();
  p.out(in_stream, out_stream, 2);

  std::string output = out_stream.str();
  std::string expected_output = std::string("Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length\n");
  expected_output.append("Bus 750: 5 stops on route, 3 unique stops, 20939.48 route length\n");
  expected_output.append("Bus 751: not found\n");
  expected_output.append("Stop Samara: not found\n");
  expected_output.append("Stop Prazhskaya: no buses\n");
  expected_output.append("Stop Biryulyovo Zapadnoye: buses 256 828\n");
  ASSERT_EQUAL(output, expected_output);
}

void TestAll() {
  TestRunner tr;
  RUN_TEST(tr, TestParseInputRequest);
  RUN_TEST(tr, TestRunProcessor_A);
  RUN_TEST(tr, TestRunProcessor_B);
}
