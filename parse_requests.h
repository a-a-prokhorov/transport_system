#pragma once

#include "stops.h"
#include "buses.h"

#include <string>
#include <variant>

std::variant<Stop, Bus> ParseInputRequest(std::string &line);

std::string ParseOutputRequest(std::string &line);
