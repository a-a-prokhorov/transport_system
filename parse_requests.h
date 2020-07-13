#pragma once

#include "stops.h"
#include "buses.h"

#include <string>
#include <variant>

std::variant<Stop, Bus> ParseInputRequest(std::string &line);

std::variant<Stop, Bus>  ParseOutputRequest(std::string &line);
