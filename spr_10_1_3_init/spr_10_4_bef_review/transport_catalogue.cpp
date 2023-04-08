#include "input_reader.h"
#include "geo.h"
#include "transport_catalogue.h"
#include <functional>

using namespace std;
namespace project_namespace {

void TransportCatalogue::AddBus(Bus b) {
	Busptr bptr;
	deque <string*> stops_ptr;
	for (auto stop : b.stops) {
		auto first_space_colon = stop.find_first_not_of(" ");
		auto last_space_colon = stop.find_last_not_of(" ");
		stop = stop.substr(first_space_colon, last_space_colon);
		auto it = stop_name_to_stop_.find(stop);
		if (it != stop_name_to_stop_.end()) {
			stops_ptr.push_back(&it->second->stop);
		}
	}
	bptr.bus = b.bus;
	bptr.type = b.type;  // sv
	bptr.stops = stops_ptr;
	buses_.push_back(bptr); //move
	Busptr* bptr_bus = &buses_.back();
	bus_name_to_bus_.emplace(b.bus, bptr_bus);
	for (auto el : stops_ptr) {
		stop_info_[*el].insert(b.bus);
	}
}

void TransportCatalogue::AddStop(Stop s) {
	stops_.push_back(s); //move
	Stop* ptr_stop = &stops_.back();
	stop_name_to_stop_.emplace(s.stop, ptr_stop);
}

Busptr TransportCatalogue::FindBus(string bus) {
	Busptr res;
	if (bus_name_to_bus_.count(bus)) { return *bus_name_to_bus_[bus]; }
	else { return res; }
}


Stop* TransportCatalogue::FindStop(string stop) {
	Stop* res =nullptr;
	if (stop_name_to_stop_.count(stop)) { return stop_name_to_stop_[stop]; }
	else { return res; }
}


AllBusInfoBusResponse TransportCatalogue::GetAllBusInfo(string bus) {
	AllBusInfoBusResponse all_r;
	Busptr fb = FindBus(bus);
	deque<string*> stops_v = fb.stops;
	double coord_length = 0;
	int real_length = 0;
	if (stops_v.size() != 0) {
		all_r.bus = bus;

		if (fb.type == "c"s) {
			all_r.stops = stops_v.size();
			unordered_set<string*> us(stops_v.begin(), stops_v.end());
			all_r.uniq_stops = us.size();
			for (int i = 0; i < stops_v.size() - 1; i++) {
				Stop* one = FindStop(*stops_v[i]);
				Stop* two = FindStop(*stops_v[i + 1]);
				coord_length += detail::ComputeDistance(one->coordinates, two->coordinates);
				real_length += GetStopDistance(make_pair(one, two));
			}
		}
		else {
			all_r.stops = stops_v.size() * 2 - 1;
			unordered_set<string*> us(stops_v.begin(), stops_v.end());
			all_r.uniq_stops = us.size();
			for (int i = 0; i < stops_v.size() - 1; i++) {
				Stop* one = FindStop(*stops_v[i]);
				Stop* two = FindStop(*stops_v[i + 1]);
				coord_length += detail::ComputeDistance(one->coordinates, two->coordinates);
				real_length += GetStopDistance(make_pair(one, two));
			}
			for (auto it = stops_v.rbegin(); it != stops_v.rend(); ++it) {
				if (it != stops_v.rbegin()) {

					Stop* two = FindStop(**it);
					Stop* one = FindStop(**(it-1));

					string st_one_name = one->stop;
					string st_two_name = two->stop;

					int new_length = GetStopDistance(make_pair( one, two));

					real_length += new_length;
				}
			}
			coord_length += coord_length;
		}

		all_r.r_length = real_length;
		all_r.curv = real_length / coord_length;
	}
	else {
		all_r.bus = bus; all_r.stops = 0;
	}
	return all_r;
}
 
set<string> TransportCatalogue::GetStopInfo(string s) {
	set<string> res;
	if (stop_info_.count(s)) {
		return stop_info_[s];
	}
	else { return res; }

}

void TransportCatalogue::AddStopDistance(Stop s) {
	if (s.stop_dist.size() != 0) {
		Stop* main_stop_ptr = stop_name_to_stop_[s.stop];
		vector<pair<string, int>> stop_dist_main = s.stop_dist;
		for (auto el : stop_dist_main) {
			Stop* another_stop_ptr = stop_name_to_stop_[el.first];
			int distance = el.second;
			stops_distance_.emplace(make_pair(main_stop_ptr, another_stop_ptr), distance);
		}
	}
}


int TransportCatalogue::GetStopDistance(std::pair<Stop*, Stop*> p) {
	auto it = stops_distance_.find(p);
	auto it2 = stops_distance_.find(make_pair(p.second, p.first));

	if (it != stops_distance_.end()) {
		return stops_distance_[p];
	}
	else if (it2 != stops_distance_.end()) {
		return stops_distance_[make_pair(p.second, p.first)];
	}
	else { return 0; }
}
}