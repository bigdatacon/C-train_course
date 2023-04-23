#pragma once
#include <cassert>
#include <chrono>
#include <sstream>
#include <string_view>

#include "json.h"

using namespace json;
using namespace std::literals;
using namespace std;



json::Document LoadJSON(const std::string& s); 

std::string Print(const Node& node); 

void ReadInputJsonRequest(); 
