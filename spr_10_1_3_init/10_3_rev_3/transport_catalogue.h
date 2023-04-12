#pragma once

#include <string>
#include <algorithm>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <math.h>
#include "geo.h"
#include <functional> 

namespace transport_catalogue {

	struct Stop {
		std::string stop_name;
		geo::Coordinates coordinates;
		std::vector<std::pair<std::string, int>> stop_dist;
	};

	struct BusDescription {
		std::string bus_name;
		std::vector<std::string> stops;
		std::string type;
	};
	
	struct AllBusInfoBusResponse {
	std::string bus_name;
	int quant_stops;
	int quant_uniq_stops;
	double route_length = 0;
	double route_curvature = 0;
};



	struct Bus {
		std::string bus_name;
		std::deque<std::string*> stops;
		std::string type;
	};

	namespace detail {
		struct PairOfStopPointerUsing_hash {
			std::size_t operator()(const std::pair<Stop*, Stop*>& p) const {
				std::size_t hash1 = std::hash<Stop*>{}(p.first);
				std::size_t hash2 = std::hash<Stop*>{}(p.second);
				std::size_t combined_hash = hash1 + hash2;
				return combined_hash;
			}
		};

		struct PairOfStopPointerUsingString {
			std::size_t operator()(const std::pair<Stop*,   Stop*>& p) const {
				const std::size_t constantal = 31;
				std::size_t hash_value_one = 0;
				std::size_t hash_value_two = 0;
				std::size_t p_pow = 1;
				for (char c : p.first->stop_name) {
					hash_value_one = (hash_value_one + (c - 'a' + 1) * p_pow) % std::size_t(-1);
					p_pow = (p_pow * constantal) % std::size_t(-1);
				}

				for (char c : p.second->stop_name) {
					hash_value_one = (hash_value_two + (c - 'a' + 1) * p_pow) % std::size_t(-1);
					p_pow = (p_pow * constantal) % std::size_t(-1);
				}


				return hash_value_one + hash_value_two * constantal;
			}
		};
	}


	class TransportCatalogue {
	public:
		void AddBus(const BusDescription& bus);
		void AddStop(Stop stop);
		Bus* FindBus(std::string bus) const;
		const Stop* FindStop(const std::string* stop) const;
		AllBusInfoBusResponse GetAllBusInfo(std::string_view bus);

		std::set<std::string> GetStopInfo(std::string_view s) const;

		void AddStopDistance(Stop s);
		int GetStopDistance( Stop* s1, Stop* s2)  ;
	

	private:
		std::deque<Bus> buses_;
		std::deque<Stop> stops_;
		std::unordered_map<std::string, Stop*> stop_name_to_stop_;
		std::unordered_map<std::string_view, Bus*> bus_name_to_bus_;

		std::unordered_map<std::string_view, std::set<std::string>> stop_info_;
		std::unordered_map<std::pair<Stop*, Stop*>, int, detail::PairOfStopPointerUsingString> stops_distance_;
	};
}
