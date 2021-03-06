#pragma once

#include "test_runner.h"
#include "parse_requests.h"
#include "processor.h"
#include "json.h"

#include <variant>
#include <sstream>

void TestParseInputRequest() {
  {
    std::string test_input_stop = "Stop Tolstopaltsevo: 55.611087, 37.20829";
    Stop stop = std::get<Stop>(ParseInputRequest(test_input_stop));
    Stop stop_expected = Stop("Tolstopaltsevo", 55.611087, 37.20829);
    ASSERT_EQUAL(stop.GetName(), stop_expected.GetName());
    ASSERT_EQUAL(stop.GetLat(), stop_expected.GetLat());
    ASSERT_EQUAL(stop.GetLon(), stop_expected.GetLon());
  }

  {
    std::string test_input_bus = "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka";
    Bus bus = std::get<Bus>(ParseInputRequest(test_input_bus));
    Bus bus_expected = Bus("750", { "Tolstopaltsevo", "Marushkino", "Rasskazovka" }, false);
    ASSERT_EQUAL(bus.GetName(), bus_expected.GetName());
    ASSERT_EQUAL(bus.GetIsRing(), bus_expected.GetIsRing());
    std::vector<std::string> bus_expected_stops = bus_expected.GetStops();
    std::vector<std::string> bus_stops = bus.GetStops();
    for (size_t i = 0; i < bus_expected_stops.size(); ++i) {
      ASSERT_EQUAL(bus_stops[i], bus_expected_stops[i]);
    }
  }

  {
    std::string test_input_bus = "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye";
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

    {
      std::string test_input_stop = "Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam";
      Stop stop = std::get<Stop>(ParseInputRequest(test_input_stop));
      Stop stop_expected = Stop("Biryulyovo Zapadnoye", 55.574371, 37.6517, {{"Rossoshanskaya ulitsa", 7500}, {"Biryusinka", 1800}, {"Universam", 2400}});
      ASSERT_EQUAL(stop.GetName(), stop_expected.GetName());
      ASSERT_EQUAL(stop.GetLat(), stop_expected.GetLat());
      ASSERT_EQUAL(stop.GetLon(), stop_expected.GetLon());
      ASSERT_EQUAL(stop.distances_to_other_stops_.at("Rossoshanskaya ulitsa"), stop_expected.distances_to_other_stops_.at("Rossoshanskaya ulitsa"));
      ASSERT_EQUAL(stop.distances_to_other_stops_.at("Biryusinka"), stop_expected.distances_to_other_stops_.at("Biryusinka"));
      ASSERT_EQUAL(stop.distances_to_other_stops_.at("Universam"), stop_expected.distances_to_other_stops_.at("Universam"));
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

void TestRunProcessor_C() {
  Processor p;
  std::string s = std::string("13\n");
  s.append("Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino\n");
  s.append("Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka\n");
  s.append("Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n");
  s.append("Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka\n");
  s.append("Stop Rasskazovka: 55.632761, 37.333324\n");
  s.append("Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam\n");
  s.append("Stop Biryusinka: 55.581065, 37.64839, 750m to Universam\n");
  s.append("Stop Universam: 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya\n");
  s.append("Stop Biryulyovo Tovarnaya: 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya\n");
  s.append("Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye\n");
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
  p.out(in_stream, out_stream, 6);

  std::string output = out_stream.str();
  std::string expected_output = std::string("Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.361239 curvature\n");
  expected_output.append("Bus 750: 5 stops on route, 3 unique stops, 27600 route length, 1.318084 curvature\n");
  expected_output.append("Bus 751: not found\n");
  expected_output.append("Stop Samara: not found\n");
  expected_output.append("Stop Prazhskaya: no buses\n");
  expected_output.append("Stop Biryulyovo Zapadnoye: buses 256 828\n");
  ASSERT_EQUAL(output, expected_output);
}

void TestRunProcessor_C_additional() {
  Processor p;
  std::string s = "16\n"
  "Bus Looped_Single_Intrastop_Distance: A > B > C > A\n"
  "Stop A: 55.574371, 37.6517, 100m to B\n"
  "Stop B: 55.581065, 37.64839, 200m to C\n"
  "Stop C: 55.587655, 37.645687, 300m to A\n"

  "Bus Looped_Double_Intrastop_Distance: D > E > F > D\n"
  "Stop D: 55.574371, 37.6517, 100m to E, 400m to F\n"
  "Stop E: 55.581065, 37.64839, 200m to F, 500m to D\n"
  "Stop F: 55.587655, 37.645687, 300m to D, 600m to E\n"

  "Bus Straight_Single_Intrastop_Distance: G - H - I\n"
  "Stop G: 55.574371, 37.6517, 100m to H\n"
  "Stop H: 55.581065, 37.64839, 200m to I\n"
  "Stop I: 55.587655, 37.645687, 300m to H\n"

  "Bus Straight_Double_Intrastop_Distance: J - K - L\n"
  "Stop J: 55.574371, 37.6517, 100m to K, 400m to L\n"
  "Stop K: 55.581065, 37.64839, 200m to L, 500m to J\n"
  "Stop L: 55.587655, 37.645687, 300m to J, 600m to K\n"

  "4\n"
  "Bus Looped_Single_Intrastop_Distance\n"
  "Bus Looped_Double_Intrastop_Distance\n"
  "Bus Straight_Single_Intrastop_Distance\n"
  "Bus Straight_Double_Intrastop_Distance";

  std::istringstream in_stream(s);
  std::ostringstream out_stream;
  p.in(in_stream);
  p.process();
  p.out(in_stream, out_stream, 6);

  std::string output = out_stream.str();
  std::string expected_output = std::string("Bus Looped_Single_Intrastop_Distance: 4 stops on route, 3 unique stops, 600 route length, 0.196736 curvature\n");
  expected_output.append("Bus Looped_Double_Intrastop_Distance: 4 stops on route, 3 unique stops, 600 route length, 0.196736 curvature\n");
  expected_output.append("Bus Straight_Single_Intrastop_Distance: 5 stops on route, 3 unique stops, 700 route length, 0.229497 curvature\n");
  expected_output.append("Bus Straight_Double_Intrastop_Distance: 5 stops on route, 3 unique stops, 1400 route length, 0.458993 curvature\n");

  ASSERT_EQUAL(output, expected_output);
}

void TestJson() {
  Processor p;

std::string s = R"!({
"base_requests":
[{"type": "Stop", "road_distances": {"Marushkino": 3900}, "longitude": 37.20829, "name": "Tolstopaltsevo", "latitude": 55.611087},
 {"type": "Stop", "road_distances": {"Rasskazovka": 9900}, "longitude": 37.209755, "name": "Marushkino", "latitude": 55.595884},
 {"type": "Bus", "name": "256", "stops": ["Biryulyovo Zapadnoye", "Biryusinka", "Universam", "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya", "Biryulyovo Zapadnoye"], "is_roundtrip": true},
 {"type": "Bus", "name": "750", "stops": ["Tolstopaltsevo", "Marushkino", "Rasskazovka"], "is_roundtrip": false},
 {"type": "Stop", "road_distances": {}, "longitude": 37.333324, "name": "Rasskazovka", "latitude": 55.632761},
 {"type": "Stop", "road_distances": {"Rossoshanskaya ulitsa": 7500, "Biryusinka": 1800, "Universam": 2400}, "longitude": 37.6517, "name": "Biryulyovo Zapadnoye", "latitude": 55.574371},
 {"type": "Stop", "road_distances": {"Universam": 750}, "longitude": 37.64839, "name": "Biryusinka", "latitude": 55.581065},
 {"type": "Stop", "road_distances": {"Rossoshanskaya ulitsa": 5600, "Biryulyovo Tovarnaya": 900}, "longitude": 37.645687, "name": "Universam", "latitude": 55.587655},
 {"type": "Stop", "road_distances": {"Biryulyovo Passazhirskaya": 1300}, "longitude": 37.653656, "name": "Biryulyovo Tovarnaya", "latitude": 55.592028},
 {"type": "Stop", "road_distances": {"Biryulyovo Zapadnoye": 1200}, "longitude": 37.659164, "name": "Biryulyovo Passazhirskaya", "latitude": 55.580999},
 {'type": "Bus", "name": "828", "stops": ["Biryulyovo Zapadnoye", "Universam", "Rossoshanskaya ulitsa", "Biryulyovo Zapadnoye"], "is_roundtrip": true},
 {"type": "Stop", "road_distances": {}, "longitude": 37.605757, "name": "Rossoshanskaya ulitsa", "latitude": 55.595579},
 {"type": "Stop", "road_distances": {}, "longitude": 37.603831, "name": "Prazhskaya", "latitude": 55.611678}],

"stat_requests":
[{"type": "Bus", "name": "256", "id": 1965312327},
 {"type": "Bus", "name": "750", "id": 519139350 },
 {"type": "Bus", "name": "751", "id": 194217464},
 {"type": "Stop", "name": "Samara", "id": 746888088},
 {"type": "Stop", "name": "Prazhskaya", "id": 65100610},
 {"type": "Stop", "name": "Biryulyovo Zapadnoye","id": 1042838872}]})!";

std::string expected_output = R"!([
{
"route_length": 5950,
"request_id": 1965312327,
"curvature": 1.361239,
"stop_count": 6,
"unique_stop_count": 5
},
{
"route_length": 27600,
"request_id": 519139350,
"curvature": 1.318084,
"stop_count": 5,
"unique_stop_count": 3
},
{
"request_id": 194217464,
"error_message": "not found"
},
{
"request_id": 746888088,
"error_message": "not found"
},
{
"buses": [],
"request_id": 65100610
},
{
"buses": [
"256",
"828"
],
"request_id": 1042838872
}
]
)!";

  std::istringstream in_stream(s);
  std::ostringstream out_stream;
  p.inJson(in_stream);
  p.process();
  p.outJson(out_stream, 6);
  std::string output = out_stream.str();
  ASSERT_EQUAL(output, expected_output);
}

void TestAll() {
  TestRunner tr;
  RUN_TEST(tr, TestParseInputRequest);
//  RUN_TEST(tr, TestRunProcessor_A); // other output conditions
//  RUN_TEST(tr, TestRunProcessor_B); // other output conditions
  RUN_TEST(tr, TestRunProcessor_C);
  RUN_TEST(tr, TestRunProcessor_C_additional);
  RUN_TEST(tr, TestJson);
}
