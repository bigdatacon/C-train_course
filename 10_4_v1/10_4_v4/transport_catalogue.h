#pragma once

#include <string>
#include <unordered_set>
#include "geo.h"
#include <functional> 

namespace transport_catalogue {

	struct StopDistancesDescription {
		std::string stop_name;
		std::vector<std::pair<std::string, int>> distances;
	};

	struct Stop {
		std::string stop_name;
		geo::Coordinates coordinates;

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
		struct PairOfStopPointerUsingString {
			std::size_t operator()(const std::pair<const  Stop*, const  Stop*>& p) const {
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
		const Bus* FindBus(std::string_view bus) const;
		const Stop* FindStop(std::string_view stop) const;
		AllBusInfoBusResponse GetAllBusInfo(std::string_view bus) const;
		std::set<std::string> GetStopInfo(std::string_view s) const;
		void AddStopDistance(StopDistancesDescription distance);
		int GetStopDistance(const Stop& s1, const Stop& s2)  const;

	private:
		std::deque<Bus> buses_;
		std::deque<Stop> stops_;
		std::unordered_map<std::string_view, Stop*> stop_name_to_stop_;
		std::unordered_map<std::string_view, Bus*> bus_name_to_bus_;
		std::unordered_map<std::string_view, std::set<std::string>> stop_info_;
		std::unordered_map<std::pair<const Stop*, const Stop*>, int, detail::PairOfStopPointerUsingString> stops_distance_;
	};
}
