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

#include "transport_catalogue.h"
#include "geo.h"

namespace project_namespace {

std::vector<std::pair<std::string, int>> SplitStringByForDist(std::string distance_section);

std::vector<std::string> SplitStringBySignPlain(std::string str);

std::vector<std::string> SplitStringBySignCircle(std::string str);

std::pair<double, double> SplitStringByComma(std::string str);


class InputReader {
public:
	InputReader(std::istream& is);

	int GetNumUpdateQueries();

	void FillRequests();

	void GetUpdStop(TransportCatalogue& tc);

	void GetUpdBus(TransportCatalogue& tc);

	void GetUpdStopDist(TransportCatalogue& tc);



private:
	std::istream& is_;
	std::deque<Bus> upd_req_bus_;
	std::deque<Stop> upd_req_stop_;
	int num_update_q_;
};
   
}