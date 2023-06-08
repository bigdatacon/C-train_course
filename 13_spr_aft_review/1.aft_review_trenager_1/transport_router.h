
#pragma once

#include "ranges.h"
#include "router.h"
#include "graph.h"
#include "transport_catalogue.h"
#include <iostream>
#include <utility>
#include <unordered_map>
#include <map>
#include <vector>
#include <algorithm>
#include "iostream"
#include <string_view>




//using namespace domain;

namespace graph {

	struct BusActivity {
		std::string bus_name;
		double time;
		int span_count;
		std::string type_activity = "Bus";
	};


	struct WaitingActivity {
		std::string stop_name_from;
		double time;
		std::string type_activity = "Wait";
	};

	class ActivityProcessor {

	public:
		ActivityProcessor(transport_catalogue::TransportCatalogue& tc)
			: tc(tc) {
			graph_ = DirectedWeightedGraph<double>(2 * tc.GetStopsQuantity());
			AddKnots();

			router_ = std::unique_ptr<graph::Router<double>>(new graph::Router<double>(graph_));
		}

		void AddKnots() {
			const std::deque<domain::Bus>& buses_ = tc.GetBuses();

			for (const domain::Bus& bus : buses_) {
				std::string type = bus.type;
				std::deque<std::string_view> stops = bus.stops;

				if (type == "true") {
					AddStopsOneDirection(stops, bus.bus_name);
				}
				else {
					AddStopsNonRoundTrip(stops, bus.bus_name);
				}

			}
		}


		std::vector<std::variant<BusActivity, WaitingActivity>> GetRouteAndBuses(std::string_view stop_name_from, std::string_view stop_name_to) {
			size_t from = GetValueByKey(stop_name_from).value();
			size_t to = GetValueByKey(stop_name_to).value();
			std::vector<std::variant<BusActivity, WaitingActivity>> final_route;

			std::optional<typename graph::Router<double>::RouteInfo> route_info = router_->BuildRoute(from, to);

			double wait_time = tc.GetWaitTime();

			std::set<std::string> actual_buses;
			std::set<std::string> buses_for_route_first_stop;
			std::set<std::string> buses_for_route_second_stop;

			if (route_info.has_value()) {
				const auto& route_info_value = route_info.value();

				for (auto it = route_info_value.edges.begin(); it != route_info_value.edges.end(); ++it) {
					auto EdgId = *it;
					auto Edge = graph_.GetEdge(EdgId);
					if (Edge.stop_count == 0) {
						WaitingActivity wa;
						wa.time = wait_time;
						wa.stop_name_from = Edge.name;

						final_route.push_back(wa);

					}

					else {
						BusActivity ba;
						ba.bus_name = Edge.name;
						ba.time = Edge.weight;
						ba.span_count = Edge.stop_count;
						final_route.push_back(ba);

					}

				}
				return final_route;
			}
			else {
				BusActivity ba;
				ba.bus_name = "not found";
				final_route.push_back(ba);

				return final_route;
			}

		}
        
        		void FreeMemory() {
			for (Edge<double>* edge : added_edges_) {
				delete edge;
			}
			added_edges_.clear();
		}

	private:
		transport_catalogue::TransportCatalogue& tc;
		std::vector<Edge<double>*> added_edges_;
		DirectedWeightedGraph<double> graph_;
		std::unordered_map<std::string_view, size_t> stop_to_vertex_;
		std::unique_ptr<graph::Router<double>> router_;


		bool containsElement(const std::vector<Edge<double>*>& container, const Edge<double>* some) {
			return std::find(container.begin(), container.end(), some) != container.end();
		}


		std::optional<size_t> GetValueByKey(std::string_view key) {
			auto it = stop_to_vertex_.find(key);
			if (it != stop_to_vertex_.end()) {
				return it->second;  // Возвращаем значение, связанное с ключом
			}
			else {
				return std::nullopt;  // Ключ не найден
			}
		}



		void AddStopsOneDirection(const std::deque<std::string_view>& stops, const std::string& bus_name) {

			for (auto it = stops.begin(); std::next(it) != stops.end(); ++it) {
				double sum_time = 0; // накапливаемое время движения по ходу маршрута с каждой следующей остановкой сюда 
				size_t num_vertex_1_wait;
				size_t num_vertex_next_wait;

				auto num_vertex_1_wait_optional = GetValueByKey(*it);
				if (num_vertex_1_wait_optional.has_value()) {
					num_vertex_1_wait = num_vertex_1_wait_optional.value();
				}
				else {
					num_vertex_1_wait = stop_to_vertex_.size() * 2;
					stop_to_vertex_.insert({ *it , num_vertex_1_wait });

				}


				auto num_vertex_next_wait_optional = GetValueByKey(*std::next(it));
				if (num_vertex_next_wait_optional.has_value()) {
					num_vertex_next_wait = num_vertex_next_wait_optional.value();
				}
				else {
					num_vertex_next_wait = stop_to_vertex_.size() * 2;
					stop_to_vertex_.insert({ *std::next(it), num_vertex_next_wait });

				}



				size_t num_vertex1_go = num_vertex_1_wait + 1;


				const domain::Stop* stop_1 = tc.FindStop(*it);
				const domain::Stop* stop_1_next = tc.FindStop(*std::next(it));
				int distance_inner = tc.GetStopDistance(*stop_1, *stop_1_next); // расстояние от остановки 
				double time_inner = distance_inner / (tc.GetVelocity() * 1000 / 60) + sum_time;

				Edge<double>* edge_mirror = new Edge<double>{ num_vertex_1_wait, num_vertex1_go, tc.GetWaitTime() , std::string(*it), 0 };  // добавляю ребро зеркало для первой остановки 
				Edge<double>* edge_go_wait = new Edge<double>{ num_vertex1_go, num_vertex_next_wait, time_inner, bus_name, 1 };  // добавляю ребро зеркало для первой остановки 

				graph_.AddEdge(*edge_mirror);
				added_edges_.push_back(edge_mirror);

				graph_.AddEdge(*edge_go_wait);
				added_edges_.push_back(edge_go_wait);

				sum_time = time_inner;

				for (auto it_inner = std::next(it); std::next(it_inner) != stops.end(); ++it_inner) {

					size_t num_vertex_inner_next_wait;

					auto num_vertex_inner_next_wait_optional = GetValueByKey(*std::next(it_inner));
					if (num_vertex_inner_next_wait_optional.has_value()) {
						num_vertex_inner_next_wait = num_vertex_inner_next_wait_optional.value();
					}
					else {
						num_vertex_inner_next_wait = stop_to_vertex_.size() * 2;
						stop_to_vertex_.insert({ *std::next(it_inner), num_vertex_inner_next_wait });

					}



					const domain::Stop* stop_inner = tc.FindStop(*it_inner);
					const domain::Stop* stop_inner_next = tc.FindStop(*(std::next(it_inner)));
					int distance_1_2 = tc.GetStopDistance(*stop_inner, *stop_inner_next); // расстояние от остановки 


					double time_min_1_2 = distance_1_2 / (tc.GetVelocity() * 1000 / 60) + sum_time; // увиличиваю время на то что уже было накоплено пока ехал до этой остановки 

					int span_count = std::distance(stops.begin(), std::next(it_inner)) - std::distance(stops.begin(), it);

					Edge<double>* edge_go_wait_inner = new Edge<double>{ num_vertex1_go, num_vertex_inner_next_wait , time_min_1_2, bus_name, span_count };

					graph_.AddEdge(*edge_go_wait_inner);
					added_edges_.push_back(edge_go_wait_inner);
					sum_time = time_min_1_2; // присваиваю значение накопленного времени - текущее потраченное время 

				}
			}

		}


		void AddStopsNonRoundTrip(std::deque<std::string_view> stops, const std::string& bus_name) {  // Тут если передать stops по константной ссылке то ошибка в utility
			AddStopsOneDirection(stops, bus_name); // Заполняю в прямом направлении
			std::reverse(stops.begin(), stops.end()); // разворачияю списов остановок 
			AddStopsOneDirection(stops, bus_name); // Заполняю в обратном направлении

		}
	};
}
