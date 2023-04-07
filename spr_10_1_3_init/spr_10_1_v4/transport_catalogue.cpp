#include "input_reader.h"
#include "geo.h"
#include "transport_catalogue.h"

using namespace std;



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

Stop TransportCatalogue::FindStop(string stop) {
	Stop res;
	if (stop_name_to_stop_.count(stop)) { return *stop_name_to_stop_[stop]; }
	else { return res; }
}

AllBusInfoBusResponse TransportCatalogue::GetAllBusInfo(string bus) {
	AllBusInfoBusResponse all_r;
	Busptr fb = FindBus(bus);
	deque<string*> stops_v = fb.stops;
	if (stops_v.size() != 0) {
		all_r.bus = bus;
		if (fb.type == "c"s) {
			all_r.stops = stops_v.size();
			unordered_set<string*> us(stops_v.begin(), stops_v.end());
			all_r.uniq_stops = us.size();
			for (int i = 0; i < stops_v.size() - 1; i++) {
				Stop one = FindStop(*stops_v[i]);
				Stop two = FindStop(*stops_v[i + 1]);
				all_r.r_length += ComputeDistance(one.coordinates, two.coordinates);
			}
		}
		else {
			all_r.stops = stops_v.size() * 2 - 1;
			all_r.uniq_stops = stops_v.size();
			for (int i = 0; i < stops_v.size() - 1; i++) {
				Stop one = FindStop(*stops_v[i]);
				Stop two = FindStop(*stops_v[i + 1]);
				all_r.r_length += ComputeDistance(one.coordinates, two.coordinates);
			}
			all_r.r_length += all_r.r_length;
		}
	}
	else {
		all_r.bus = bus; all_r.stops = 0;
	}
	return all_r;
}


set<string> TransportCatalogue::GetStopInfo(string s) {
	set<string> stop_buses;
	for (auto el: bus_name_to_bus_) {
		for (string* strPtr : el.second.stops) {
			// проверяем указатель на соответствие искомой строке
			if (*strPtr == s) {
				stop_buses.insert(el.second.bus);
			}
		}
	}
	return stop_buses;


}
