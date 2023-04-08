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
	std::unordered_set<std::pair<std::string, int>> stop_dist;
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
	double curv = 0;
};

struct Busptr {
	std::string bus;
	std::deque<std::string*> stops;
	std::string type;
};

struct PairHash {
	std::size_t operator()(const std::pair<Stop*, Stop*>& p) const {
		std::size_t hash1 = std::hash<Stop*>{}(p.first);
		std::size_t hash2 = std::hash<Stop*>{}(p.second);
		std::size_t combined_hash = hash1 + hash2;
		return combined_hash;
	}
};


class TransportCatalogue {
public:
	void AddBus(Bus b);
	void AddStop(Stop s); 
	Busptr FindBus(std::string bus);
	Stop* FindStop(std::string stop);
	//Stop FindStop(std::string stop);
	AllBusInfoBusResponse GetAllBusInfo(std::string bus);

	std::set<std::string> GetStopInfo(std::string s);

	void AddStopDistance(Stop s);
	int GetStopDistance(std::pair<Stop*, Stop*> p);

private:
	std::deque<Busptr> buses_;
	std::deque<Stop> stops_;
	std::unordered_map<std::string, Stop*> stop_name_to_stop_;
	std::unordered_map<std::string, Busptr*> bus_name_to_bus_;

	std::unordered_map<std::string, std::set<std::string>> stop_info_;
	std::unordered_map<std::pair<Stop*, Stop*>, int, PairHash> stops_distance_;
};