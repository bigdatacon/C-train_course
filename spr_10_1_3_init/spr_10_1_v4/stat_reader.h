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
#include <utility>
#include <math.h>
#include <regex>

#include "geo.h"
#include "transport_catalogue.h"


//std::ostream& operator<<(std::ostream& os, const AllBusInfoBusResponse& r); 
void operator<<(ostream& os, const set<string>& r);

struct StatR {
	std::string name;
	std::string str;
};

class StatReader {
public:
	StatReader(std::istream& is) : is_(is) {};
	

	int GetNumOutQueries(); 

	void FillRequests(); 
	void Output(TransportCatalogue& tc); 

private:
	std::istream& is_;
	std::deque<StatR> req_;
	int num_update_q_;
};
