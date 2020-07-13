#include "parse_requests.h"

#include <iostream>
#include <iomanip>

using namespace std;

variant<Stop, Bus> ParseInputRequest(string &line) {
  size_t pos_of_first_space = line.find(" ");
  size_t pos_of_colon = line.find(":", pos_of_first_space);
  string request = line.substr(0, pos_of_first_space);
  string name = line.substr(pos_of_first_space + 1, pos_of_colon - pos_of_first_space - 1);

  if (request == "Stop") {
    size_t pos_of_first_comma = line.find(",", pos_of_colon + 2);
    string lat_string = line.substr(pos_of_colon + 2, pos_of_first_comma - pos_of_colon - 2);
    size_t pos_of_next_comma_or_end = line.find(",", pos_of_first_comma + 2);
    string lon_string = line.substr(pos_of_first_comma + 2, pos_of_next_comma_or_end - pos_of_first_comma - 2);
    return move(Stop(name, stod(lat_string), stod(lon_string)));
  }
  else {
    size_t pos_of_dash = line.find("-", pos_of_colon);
    bool isRing = false;
    string delimiter;
    size_t pos_of_delimiter = pos_of_colon;
    size_t next_pos_of_delimiter;
    if (pos_of_dash != string::npos) {
      delimiter = "-";
      next_pos_of_delimiter = pos_of_dash;
    } else {
      delimiter = ">";
      next_pos_of_delimiter = line.find(delimiter, pos_of_colon);
      isRing = true;
    }

    size_t length_of_next_stop = (next_pos_of_delimiter - 1) - (pos_of_delimiter + 2);

    string stop;
    vector<string> stops;
    do {
      stop = line.substr(pos_of_delimiter + 2, length_of_next_stop);
      stops.push_back(stop);
      pos_of_delimiter = next_pos_of_delimiter;
      next_pos_of_delimiter = line.find(delimiter, pos_of_delimiter + 1);
      length_of_next_stop = (next_pos_of_delimiter - 1) - (pos_of_delimiter + 2);
    } while (pos_of_delimiter != string::npos);

    return move(Bus(name, stops, isRing));
  }
}

std::string ParseOutputRequest(std::string &line) {
  size_t pos_of_first_space = line.find(" ");
  string name = line.substr(pos_of_first_space + 1);
  return name;
}
