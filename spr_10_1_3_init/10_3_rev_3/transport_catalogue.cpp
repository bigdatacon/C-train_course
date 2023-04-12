#include "input_reader.h"
#include "geo.h"
#include <string_view>
#include "transport_catalogue.h"



using namespace std;
namespace transport_catalogue {

	void TransportCatalogue::AddBus(const BusDescription& b) {
		Bus bptr;
		deque <std::string*> stops_ptr;
		for (const auto& stop : b.stops) {   
			auto it = stop_name_to_stop_.find(stop);
			if (it != stop_name_to_stop_.end()) {
				//std::string_view stop_v = string_view( it->second->stop_name);
				stops_ptr.push_back(&it->second->stop_name);
		
			}
		}
		bptr.bus_name = b.bus_name;
		bptr.type = b.type;  // sv
		bptr.stops = stops_ptr;
		buses_.push_back(bptr); //move
		Bus* bptr_bus = &buses_.back();
		bus_name_to_bus_.emplace(b.bus_name, bptr_bus);
		for (auto el : stops_ptr) {
			stop_info_[*el].insert(b.bus_name);
		}
	}

	void TransportCatalogue::AddStop(Stop stop) {
		stops_.push_back(stop); //move нельзя использовать так как в этом случае удалится stop из контейнера который в input_reader - а по нему нужна вторая проходка 
		Stop* ptr_stop = &stops_.back();
		stop_name_to_stop_.emplace(stop.stop_name, ptr_stop);
	}

	Bus* TransportCatalogue::FindBus(string bus) const  {
		Bus* res= nullptr;
		if (bus_name_to_bus_.count(bus)) { return bus_name_to_bus_.at(bus); }
		else { return res; }
	}


	const Stop* TransportCatalogue::FindStop(const string* stop) const {
		const Stop* res = nullptr;
		if (stop_name_to_stop_.count(*stop)) { return static_cast<const Stop*>(stop_name_to_stop_.at(*stop)); }
		else { return res; }
	}


	AllBusInfoBusResponse TransportCatalogue::GetAllBusInfo(string_view bus) {
		AllBusInfoBusResponse all_r;
		Bus* fb = FindBus(string(bus));
		if (fb) {
			deque<string*> stops_v = fb->stops;
			double coord_length = 0;
			int real_length = 0;
			if (stops_v.size() != 0) {
				all_r.bus_name = bus;

				if (fb->type == "c"s) {
					all_r.quant_stops = stops_v.size();
					unordered_set<string*> us(stops_v.begin(), stops_v.end());
					all_r.quant_uniq_stops = us.size();
					for (int i = 0; i < stops_v.size() - 1; i++) {
						const Stop* one = FindStop(stops_v[i]);
						const Stop* two = FindStop(stops_v[i + 1]);
						coord_length += geo::ComputeDistance(one->coordinates, two->coordinates);
						real_length += GetStopDistance(const_cast<Stop*>(one), const_cast<Stop*>(two));
					}
				}
				else {
					all_r.quant_stops = stops_v.size() * 2 - 1;
					unordered_set<string*> us(stops_v.begin(), stops_v.end());
					all_r.quant_uniq_stops = us.size();
					for (int i = 0; i < stops_v.size() - 1; i++) {
						const Stop* one = FindStop(stops_v[i]);
						const Stop* two = FindStop(stops_v[i + 1]);
						coord_length += geo::ComputeDistance(one->coordinates, two->coordinates);
						real_length += GetStopDistance(const_cast<Stop*>(one), const_cast<Stop*>(two));
					}
					for (auto it = stops_v.rbegin(); it != stops_v.rend(); ++it) {
						if (it != stops_v.rbegin()) {

							const Stop* two = FindStop(*it);
							const Stop* one = FindStop(*(it - 1));

							string st_one_name = one->stop_name;
							string st_two_name = two->stop_name;

							int new_length = GetStopDistance(const_cast<Stop*>(one), const_cast<Stop*>(two));

							real_length += new_length;
						}
					}
					coord_length += coord_length;
				}

				all_r.route_length = real_length;
				all_r.route_curvature = real_length / coord_length;
			}
		}
		else {
			all_r.bus_name = bus; all_r.quant_stops = 0;
		}
		return all_r;
	}

	set<string> TransportCatalogue::GetStopInfo(std::string_view s) const {
		set<string> res;
		if (stop_info_.count(s)) {
			return stop_info_.at(s);
		}
		else { return res; }

	}

	void TransportCatalogue::AddStopDistance(Stop s) {
		if (s.stop_dist.size() != 0) {
			Stop* main_stop_ptr = stop_name_to_stop_[s.stop_name];
			vector<pair<string, int>> stop_dist_main = s.stop_dist;
			for (auto el : stop_dist_main) {
				Stop* another_stop_ptr = stop_name_to_stop_[el.first];
				int distance = el.second;
				stops_distance_.emplace(make_pair(main_stop_ptr, another_stop_ptr), distance);
			}
		}
	}


	int TransportCatalogue::GetStopDistance(Stop* s1, Stop* s2)  {
		auto it = stops_distance_.find(make_pair(s1, s2));
		auto it2 = stops_distance_.find(make_pair(s2, s1));

		if (it != stops_distance_.end()) {
			return stops_distance_[make_pair(s1, s2)];
		}
		else if (it2 != stops_distance_.end()) {
			return stops_distance_[make_pair(s2, s1)];
		}
		else { return 0; }
	}
}