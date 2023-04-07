#pragma once
#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <deque>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <math.h>
#include <regex>
#include "geo.h"



struct Stop {
	std::string stop;
	Coordinates coordinates;
};

struct Bus {
	std::string bus;
	std::vector<std::string> stops;
	std::string type;
};

struct AllBusInfoBusResponse {
	std::string bus;
	int stops;
	int uniq_stops;
	double r_length = 0; 
};

struct Busptr {
	std::string bus;
	std::deque<std::string*> stops;
	std::string type;
};

class TransportCatalogue {
public:
	void AddBus(Bus b);
	void AddStop(Stop s); 
	Busptr FindBus(std::string bus);
	Stop FindStop(std::string stop);
	AllBusInfoBusResponse GetAllBusInfo(std::string bus);

private:
	std::deque<Busptr> buses_;
	std::deque<Stop> stops_;
	std::unordered_map<std::string, Stop*> stop_name_to_stop_;
	std::unordered_map<std::string, Busptr*> bus_name_to_bus_;
};