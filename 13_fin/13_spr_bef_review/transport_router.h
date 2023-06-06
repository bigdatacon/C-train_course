
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




using namespace domain;

namespace graph {

	struct Activity {
		std::string stop_name_from;
		std::string stop_name_to;

		double time;
		std::string type_activity;
		std::string bus_name;
		int span_count;
	};

	template <typename Weight>
	class ActivityProcessor {

	public:
		ActivityProcessor( transport_catalogue::TransportCatalogue& tc)
			:  tc(tc) {
			graph_ = std::make_unique<DirectedWeightedGraph<Weight>>(2 * tc.GetStopSet().size());
		}

		bool containsElement(const std::vector<Edge<Weight>*>& container, const Edge<Weight>* some) {
			return std::find(container.begin(), container.end(), some) != container.end();
		}

		void AddStopsOneDirection(const std::deque<std::string_view>& stops, const std::string& bus_name) {

			for (auto it = stops.begin(); std::next(it) != stops.end(); ++it) {
				double sum_time = 0; // накапливаемое время движения по ходу маршрута с каждой следующей остановкой сюда 
				size_t num_vertex_1_wait = tc.GetStopVertexIdByName( *it) * 2;
				size_t num_vertex1_go = num_vertex_1_wait + 1;
				size_t num_vertex_next_wait = tc.GetStopVertexIdByName( *std::next(it)) * 2;

				const Stop* stop_1 = tc.FindStop(*it);
				const Stop* stop_1_next = tc.FindStop(*std::next(it));
				int distance_inner = tc.GetStopDistance(*stop_1, *stop_1_next); // расстояние от остановки 
				double time_inner = distance_inner / (tc.GetVelocity() * 1000 / 60) + sum_time;

				Edge<double>* edge_mirror = new Edge<double>{ num_vertex_1_wait, num_vertex1_go, tc.GetWaitTime() , std::string(*it), 0 };  // добавляю ребро зеркало для первой остановки 
				Edge<double>* edge_go_wait = new Edge<double>{ num_vertex1_go, num_vertex_next_wait, time_inner, bus_name, 1 };  // добавляю ребро зеркало для первой остановки 

				graph_->AddEdge(*edge_mirror);
				added_edges_.push_back(edge_mirror);

				graph_->AddEdge(*edge_go_wait);
				added_edges_.push_back(edge_go_wait);

				sum_time = time_inner;

				for (auto it_inner = std::next(it); std::next(it_inner) != stops.end(); ++it_inner) {

					size_t num_vertex_inner_next_wait = tc.GetStopVertexIdByName(*std::next(it_inner)) * 2;
					const Stop* stop_inner = tc.FindStop(*it_inner);
					const Stop* stop_inner_next = tc.FindStop(*(std::next(it_inner)));
					int distance_1_2 = tc.GetStopDistance(*stop_inner, *stop_inner_next); // расстояние от остановки 


					double time_min_1_2 = distance_1_2 / (tc.GetVelocity() * 1000 / 60) + sum_time; // увиличиваю время на то что уже было накоплено пока ехал до этой остановки 

					int span_count = std::distance(stops.begin(), std::next(it_inner)) - std::distance(stops.begin(), it);

					Edge<double>* edge_go_wait_inner = new Edge<double>{ num_vertex1_go, num_vertex_inner_next_wait , time_min_1_2, bus_name, span_count };

					graph_->AddEdge(*edge_go_wait_inner);
					added_edges_.push_back(edge_go_wait_inner);
					sum_time = time_min_1_2; // присваиваю значение накопленного времени - текущее потраченное время 

				}
			}

		}


		void AddStopsNonRoundTrip(std::deque<std::string_view> stops,  const std::string& bus_name) {  // Тут если передать stops по константной ссылке то ошибка в utility
			AddStopsOneDirection(stops,  bus_name); // Заполняю в прямом направлении
			std::reverse(stops.begin(), stops.end()); // разворачияю списов остановок 
			AddStopsOneDirection(stops,  bus_name); // Заполняю в обратном направлении

		}

		void AddKnots() {
			std::deque<domain::Bus> buses_ = tc.GetBuses();
	
			for (Bus bus : buses_) {
				std::string type = bus.type;
				std::deque<std::string_view> stops = bus.stops;

				if (type == "true") {
					AddStopsOneDirection(stops, bus.bus_name);
				}
				else {
					AddStopsNonRoundTrip(stops,  bus.bus_name);
				}

			}
		}


		graph::Router<Weight> ProcessRoute() {
			AddKnots();
			graph::Router<Weight> router(*graph_);
			return router;
		}


		std::optional<typename graph::Router<Weight>::RouteInfo> GetShortestRoute(graph::VertexId from, graph::VertexId to, graph::Router<Weight>& router) {
			return router.BuildRoute(from, to);
		}


		std::vector<Activity> GetRouteAndBuses(std::optional<typename graph::Router<Weight>::RouteInfo>& route_info) {
			double wait_time = tc.GetWaitTime();
			std::vector<Activity> final_route;
			Activity go_activity;

			std::set<std::string> actual_buses;
			std::set<std::string> buses_for_route_first_stop;
			std::set<std::string> buses_for_route_second_stop;

			if (route_info.has_value()) {
				const auto& route_info_value = route_info.value();

				for (auto it = route_info_value.edges.begin(); it != route_info_value.edges.end(); ++it) {
					auto EdgId = *it;
					auto Edge = graph_->GetEdge(EdgId);
					if (Edge.stop_count == 0) {
						Activity activity;
						activity.type_activity = "Wait";
						activity.time = wait_time;
						activity.stop_name_from = Edge.name;

						activity.span_count = 0;
						final_route.push_back(activity);

					}

					else {
						Activity activity;
						activity.bus_name = Edge.name;
						activity.type_activity = "Bus";
						activity.type_activity = "Bus";
						activity.time = Edge.weight;
						activity.span_count = Edge.stop_count;
						final_route.push_back(activity);

					}

				}
				return final_route;
			}
		

			else {
				Activity activity;
				activity.type_activity = "Not found";
				final_route.push_back(activity);

				return final_route;
			}
			
		}


	private:
		transport_catalogue::TransportCatalogue& tc;
		std::vector<Edge<Weight>*> added_edges_;
		std::unique_ptr<DirectedWeightedGraph<Weight>> graph_;
	};
}
