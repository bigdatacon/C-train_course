#include "json_reader.h"
#include "geo.h"
#include "transport_catalogue.h"
#include <cmath>


using namespace std;
using namespace domain;
namespace transport_catalogue {

	void TransportCatalogue::AddBus(const BusDescription& b) {
		Bus bptr;
		deque <std::string_view> stops_ptr;
		for (const auto& stop : b.stops) {
			auto it = stop_name_to_stop_.find(stop);
			if (it != stop_name_to_stop_.end()) {
				stops_ptr.push_back(it->second->stop_name);

			}
		}
		bptr.bus_name = b.bus_name;
		bptr.type = b.type;  // sv
		bptr.stops = stops_ptr;
		buses_.push_back(bptr); //move
		Bus* bptr_bus = &buses_.back();
		bus_name_to_bus_.emplace(b.bus_name, bptr_bus);
		for (auto el : stops_ptr) {
			stop_info_[el].insert(b.bus_name);
		}
	}


	void TransportCatalogue::AddStop(Stop stop) {
		stops_.push_back(move(stop));
		Stop* ptr_stop = &stops_.back();
		stop_name_to_stop_.emplace(string_view(ptr_stop->stop_name), ptr_stop);
		stop_set_.insert(ptr_stop);
	}


	const Bus* TransportCatalogue::FindBus(string_view bus) const {
		const Bus* res = nullptr;
		if (bus_name_to_bus_.count(bus)) { return static_cast<const Bus*>(bus_name_to_bus_.at(bus)); }
		else { return res; }
	}

	const Stop* TransportCatalogue::FindStop(std::string_view stop) const {
		const Stop* res = nullptr;

		if (stop_name_to_stop_.count(stop)) { return static_cast<const Stop*>(stop_name_to_stop_.at(stop)); }
		else { return res; }
	}


	AllBusInfoBusResponse TransportCatalogue::GetAllBusInfo(string_view bus)  const {
		AllBusInfoBusResponse all_r;
		const Bus* fb = FindBus(string(bus));
		if (fb) {
			deque<string_view> stops_v = fb->stops;
			double coord_length = 0;
			int real_length = 0;
			if (stops_v.size() != 0) {
				all_r.bus_name = bus;

				if (fb->type == "true"s) {
					all_r.quant_stops = stops_v.size();
					unordered_set<string_view> us(stops_v.begin(), stops_v.end());
					all_r.quant_uniq_stops = us.size();
					for (int i = 0; i < static_cast<int>(stops_v.size()) - 1; i++) {
						const Stop* one = FindStop(stops_v[i]);
						const Stop* two = FindStop(stops_v[i + 1]);
						coord_length += geo::ComputeDistance(one->coordinates, two->coordinates);
						real_length += GetStopDistance(*one, *two);
					}
				}
				else {
					all_r.quant_stops = stops_v.size() * 2 - 1;
					unordered_set<string_view> us(stops_v.begin(), stops_v.end());
					all_r.quant_uniq_stops = us.size();
					for (int i = 0; i < static_cast<int>(stops_v.size()) - 1; i++) {
						const Stop* one = FindStop(stops_v[i]);
						const Stop* two = FindStop(stops_v[i + 1]);
						coord_length += geo::ComputeDistance(one->coordinates, two->coordinates);
						real_length += GetStopDistance(*one, *two);
					}
					for (auto it = stops_v.rbegin(); it != stops_v.rend(); ++it) {
						if (it != stops_v.rbegin()) {

							const Stop* two = FindStop(*it);
							const Stop* one = FindStop(*(it - 1));

							string st_one_name = one->stop_name;
							string st_two_name = two->stop_name;

							int new_length = GetStopDistance(*one, *two);

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

	void TransportCatalogue::AddStopDistance(StopDistancesDescription distance) {
		if (distance.distances.size() != 0) {
			Stop* main_stop_ptr = stop_name_to_stop_[string_view(distance.stop_name)];
			vector<pair<string, int>> stop_dist_main = distance.distances;
			for (auto el : stop_dist_main) {
				Stop* another_stop_ptr = stop_name_to_stop_[el.first];
				int distance = el.second;
				stops_distance_.emplace(make_pair(main_stop_ptr, another_stop_ptr), distance);
				//std::cout << "distance : " << distance << " bus_velocity_ " << bus_velocity_ << " distance/ bus_velocity_*1000 / 60 "<< distance / (bus_velocity_ * 1000 / 60)<< endl;
				stops_distance_time_.emplace(make_pair(main_stop_ptr, another_stop_ptr), distance/( bus_velocity_*1000 / 60));
			}
		}
	}

	int TransportCatalogue::GetStopDistance(const Stop& s11, const Stop& s22) const {
		const Stop* s1 = &s11;
		const Stop* s2 = &s22;
		auto it = stops_distance_.find(make_pair(s1, s2));
		auto it2 = stops_distance_.find(make_pair(s2, s1));

		if (it != stops_distance_.end()) {
			return stops_distance_.at(make_pair(s1, s2));
		}
		else if (it2 != stops_distance_.end()) {
			return stops_distance_.at(make_pair(s2, s1));
		}
		else { return 0; }
	}

	const std::deque<Bus>& TransportCatalogue::GetBuses() const { return buses_; }
	const std::deque<Stop>& TransportCatalogue::GetStops() const { return stops_; }


	// добавлено на 13 спринт
	// заполнить скорость и время ожидания 
	void TransportCatalogue::AddRouteSettings(const domain::RouteSettings route_settings) {
		bus_wait_time_ = route_settings.bus_wait_time;
		bus_velocity_ = route_settings.bus_velocity;
	}

	// получить время ожидания на остановке
	double TransportCatalogue::GetWaitTime() { return bus_wait_time_;  }



	std::string TransportCatalogue::GetStopNameByVertexId( size_t vertex_id) {
		size_t vertex_id_actual = 0;
		if (vertex_id % 2 != 0) {
			vertex_id_actual = (vertex_id - 1) / 2;

		}
		else {
			vertex_id_actual = vertex_id / 2;

		}
		auto it = stop_set_.begin();
		std::advance(it, vertex_id_actual);
		if (it != stop_set_.end()) {
			return (*it)->stop_name;
		}
		return "";
	}


	std::set<domain::Stop*, StopPointerComparer> TransportCatalogue::GetStopSet() {
		return stop_set_;
	}



	bool TransportCatalogue::CheckExistenceOfStopByName(std::string stop_name) {
		auto it = std::find_if(stop_set_.begin(), stop_set_.end(), [&](const Stop* stop) {
			return stop->stop_name == stop_name;
			});

		if (it != stop_set_.end()) {
			//size_t index = std::distance(stop_set_.begin(), it);

			return true;
		}
		else {
			return false;
		}
	}



	size_t TransportCatalogue::GetStopVertexIdByName(std::string_view stop_name) {
		auto it = std::find_if(stop_set_.begin(), stop_set_.end(), [&](const Stop* stop) {
			return stop->stop_name == stop_name;
			});

		if (it != stop_set_.end()) {
			size_t index = std::distance(stop_set_.begin(), it);

			return index;
		}
		else {
			return 0;
		}
	}

	///////////////////////******************************Было до заполнения stop_set при добавлении остановки 
	/*
	std::string TransportCatalogue::GetStopNameByVertexId(const std::set<domain::Stop*, StopPointerComparer>& stops_set, size_t vertex_id) {
		size_t vertex_id_actual = 0;
		if (vertex_id % 2 != 0) {
			vertex_id_actual = (vertex_id - 1) / 2;

		}
		else {
			vertex_id_actual = vertex_id / 2;

		}
		auto it = stops_set.begin();
		std::advance(it, vertex_id_actual);
		if (it != stops_set.end()) {
			return (*it)->stop_name;
		}
		return "";
	}


	std::set<domain::Stop*, StopPointerComparer> TransportCatalogue::GetStopSet() {
		std::set<domain::Stop*, StopPointerComparer> stop_set;
		for (auto& stop : stops_)
		{

			stop_set.insert(&stop);
		}

		return stop_set;
	}



	bool TransportCatalogue::CheckExistenceOfStopByName(const std::set<domain::Stop*, StopPointerComparer>& stop_set, std::string stop_name) {
		auto it = std::find_if(stop_set.begin(), stop_set.end(), [&](const Stop* stop) {
			return stop->stop_name == stop_name;
			});

		if (it != stop_set.end()) {
			size_t index = std::distance(stop_set.begin(), it);

			return true;
		}
		else {
			return false;
		}
	}

	

	size_t TransportCatalogue::GetStopVertexIdByName(const std::set<domain::Stop*, StopPointerComparer>& stop_set, std::string_view stop_name) {
		auto it = std::find_if(stop_set.begin(), stop_set.end(), [&](const Stop* stop) {
			return stop->stop_name == stop_name;
			});

		if (it != stop_set.end()) {
			size_t index = std::distance(stop_set.begin(), it);

			return index;
		}
		else {
			return 0;
		}
	}
	
	*/



	//////////////////////------------- Оконачание того что было до заполнения stop_set при добавлении остановки
	
	std::unordered_map<std::pair<const domain::Stop*, const domain::Stop*>, double, detail::PairOfStopPointerUsingString> TransportCatalogue::GetstopsDistanceTime() {
		return stops_distance_time_;
	}

	double TransportCatalogue::GetVelocity() { return bus_velocity_; }



}
