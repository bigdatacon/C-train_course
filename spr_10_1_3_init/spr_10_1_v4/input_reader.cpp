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

#include "input_reader.h"
#include "transport_catalogue.h"

using namespace std;


vector<string> SplitStringBySignPlain(string str) {
	vector<string> tokens;
	stringstream ss(str);
	string token;
	while (getline(ss, token, '-'))
	{
		tokens.push_back(token);
	}
	return tokens;
}

vector<string> SplitStringBySignCircle(string str) {
	vector<string> tokens;
	stringstream ss(str);
	string token;
	while (getline(ss, token, '>'))
	{
		tokens.push_back(token);
	}
	return tokens;
}

pair<double, double> SplitStringByComma(string str) {
	pair<double, double> coordinates;
	stringstream ss(str);
	string token;

	getline(ss, token, ',');
	coordinates.first = stod(token);

	getline(ss, token, ',');
	coordinates.second = stod(token);

	return coordinates;
}


InputReader::InputReader(istream& is) : is_(is) {
}

int InputReader::GetNumUpdateQueries() {
	string line;
	getline(is_, line);
	num_update_q_ = std::stoi(line);
	return num_update_q_;
}

void InputReader::FillRequests() {
	for (int i = 0; i < num_update_q_; ++i) {
		string line;
		getline(is_, line);
		auto pos_colon = line.find(":");
		string request_section = line.substr(0, pos_colon);
		string list_section = line.substr(pos_colon + 1);
		auto space_colon = line.find(" ");
		string req_name = request_section.substr(0, space_colon);

		if (req_name == "Bus"s) {
			Bus q;
			auto circle = list_section.find(">");
			if (circle != std::string::npos) {
				vector<string> bus_stops = SplitStringBySignCircle(list_section);
				q.type = "c"s;
				q.bus = request_section.substr(space_colon + 1, pos_colon);
				q.stops = bus_stops;
				upd_req_bus_.push_back(q);
			}
			else {
				vector<string> bus_stops = SplitStringBySignPlain(list_section);
				q.type = "p"s;
				q.bus = request_section.substr(space_colon + 1, pos_colon);
				q.stops = bus_stops;
				upd_req_bus_.push_back(q);
			}
		}
		else if (req_name == "Stop"s) {
			Stop q;
			q.stop = request_section.substr(space_colon + 1, pos_colon);
			pair<double, double> coordinates = SplitStringByComma(list_section);
			q.coordinates.lat = coordinates.first;
			q.coordinates.lng = coordinates.second;
			upd_req_stop_.push_back(q);
		}
		else { continue; }
	}
}

void InputReader::GetUpdStop(TransportCatalogue& tc) {
	for (int i = 0; i < upd_req_stop_.size(); ++i) {
		//cout << "UPDT : " << upd_req_stop_[i].stop << endl;
		tc.AddStop(upd_req_stop_[i]);
	}
}

void InputReader::GetUpdBus(TransportCatalogue& tc) {
	for (int i = 0; i < upd_req_bus_.size(); ++i) {
		//cout << "UPDT BUS : " << upd_req_bus_[i].bus << endl;
		tc.AddBus(upd_req_bus_[i]);
	}
}

