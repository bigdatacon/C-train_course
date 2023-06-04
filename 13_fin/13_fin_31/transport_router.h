
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

	inline bool IsEmptyActivity(const Activity& activity) {
		return activity.stop_name_from.empty() &&
			activity.stop_name_to.empty() &&
			activity.time == 0.0 &&
			activity.type_activity.empty() && activity.bus_name.empty() && activity.span_count == 0;
	}

	inline void ClearActivity(Activity& activity) {
		activity.stop_name_from.clear();
		activity.stop_name_to.clear();
		activity.time = 0.0;
		activity.type_activity.clear();
		activity.bus_name.clear();
		activity.span_count = 0.0;
	}


	inline std::set<std::string> GetCommonElements(const std::set<std::string>& set1, const std::set<std::string>& set2) {
		std::set<std::string> result;
		std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(result, result.begin()));
		return result;
	}

	inline std::ostream& operator<<(std::ostream& os, const Activity& activity) {
		os << "From: " << activity.stop_name_from << '\n';
		os << "To: " << activity.stop_name_to << '\n';
		os << "Time: " << activity.time << '\n';
		os << "Activity type: " << activity.type_activity << '\n';
		os << "Bus name: " << activity.bus_name << '\n';
		os << "Span count: " << activity.span_count << '\n';
		return os;
	}


	template <typename Weight>
	class ActivityProcessor {


	public:

		//template <typename Weight>
		ActivityProcessor(DirectedWeightedGraph<Weight>& graph, transport_catalogue::TransportCatalogue& tc /*, graph::Router<Weight>& router*/)
			: graph(graph), tc(tc) /*, router_(router)*/ {
		}


		bool containsElement(const std::vector<Edge<Weight>>& container, const Edge<Weight>& some) {
			return std::find(container.begin(), container.end(), some) != container.end();
		}


		void AddStopsOneDirection(std::deque<std::string_view> stops, std::set<domain::Stop, transport_catalogue::StopComparer> stop_set, std::string bus_name) {

			for (auto it = stops.begin(); std::next(it) != stops.end(); ++it) {
				double sum_time = 0; // накапливаемое время движения по ходу маршрута с каждой следующей остановкой сюда 
				size_t num_vertex_1_wait = tc.GetStopVertexIdByName(stop_set, std::string(*it)) * 2;
				size_t num_vertex1_go = num_vertex_1_wait + 1;
				size_t num_vertex_next_wait = tc.GetStopVertexIdByName(stop_set, std::string(*std::next(it))) * 2;
				size_t num_vertex_next_go = num_vertex_next_wait + 1;

				const Stop* stop_1 = tc.FindStop(*it);
				const Stop* stop_1_next = tc.FindStop(*std::next(it));
				int distance_inner = tc.GetStopDistance(*stop_1, *stop_1_next); // расстояние от остановки 
				double time_inner = distance_inner / (tc.GetVelocity() * 1000 / 60) + sum_time;

				Edge<double> edge_mirror = { num_vertex_1_wait, num_vertex1_go, tc.GetWaitTime() , std::string(*it), 0 };  // добавляю ребро зеркало для первой остановки 
				Edge<double> edge_go_wait = { num_vertex1_go, num_vertex_next_wait, time_inner, bus_name, 1 };  // добавляю ребро зеркало для первой остановки 



				if (!containsElement(added_edges_, edge_mirror)) {
					graph.AddEdge(edge_mirror);
					added_edges_.push_back(edge_mirror);
				}
				if (!containsElement(added_edges_, edge_go_wait)) {
					graph.AddEdge(edge_go_wait);
					added_edges_.push_back(edge_go_wait);
				}

				sum_time = time_inner;


				// ребра на "ЕХАТЬ из останвовки во внешнем цикле добавяю во внутреннем"
				for (auto it_inner = std::next(it); std::next(it_inner) != stops.end(); ++it_inner) {

					size_t num_vertex_inner_next_wait = tc.GetStopVertexIdByName(stop_set, std::string(*std::next(it_inner))) * 2;
					//size_t num_vertex_inner_next_go = num_vertex_inner_next_wait + 1;

					const Stop* stop_inner = tc.FindStop(*it_inner);
					const Stop* stop_inner_next = tc.FindStop(*(std::next(it_inner)));
					int distance_1_2 = tc.GetStopDistance(*stop_inner, *stop_inner_next); // расстояние от остановки 


					double time_min_1_2 = distance_1_2 / (tc.GetVelocity() * 1000 / 60) + sum_time; // увиличиваю время на то что уже было накоплено пока ехал до этой остановки 

					int span_count = std::distance(stops.begin(), std::next(it_inner)) - std::distance(stops.begin(), it);


					Edge<double> edge_go_wait_inner = { num_vertex1_go, num_vertex_inner_next_wait , time_min_1_2, bus_name, span_count };
					if (!containsElement(added_edges_, edge_go_wait_inner)) {
						graph.AddEdge(edge_go_wait_inner);
						added_edges_.push_back(edge_go_wait_inner);
					}

					sum_time = time_min_1_2; // присваиваю значение накопленного времени - текущее потраченное время 

				}
			}

		}




		void AddStopsNonRoundTrip(std::deque<std::string_view> stops, std::set<domain::Stop, transport_catalogue::StopComparer> stop_set, std::string bus_name) {
			AddStopsOneDirection(stops, stop_set, bus_name); // Заполняю в прямом направлении

			std::reverse(stops.begin(), stops.end()); // разворачияю списов остановок 

			AddStopsOneDirection(stops, stop_set, bus_name); // Заполняю в обратном направлении

		}

		void AddKnots() {
			std::deque<domain::Bus> buses_ = tc.GetBuses();
			std::set<domain::Stop, transport_catalogue::StopComparer> stop_set = tc.GetStopSet();


			for (Bus bus : buses_) {
				std::string type = bus.type;
				std::deque<std::string_view> stops = bus.stops;

				if (type == "true") {

					AddStopsOneDirection(stops, stop_set, bus.bus_name);
				}
				else {
					std::cout << "Non_round_TRIP" << std::endl;

					AddStopsNonRoundTrip(stops, stop_set, bus.bus_name);
				}

			}
		}

		//template <typename Weight>
		graph::Router<Weight> ProcessRoute() {
			std::cout << "start";
			AddKnots();
			graph::Router<Weight> router(graph);
			//router_(graph);
			return router;
		}

		//template <typename Weight>



		std::optional<typename graph::Router<Weight>::RouteInfo> GetShortestRoute(graph::VertexId from, graph::VertexId to, graph::Router<Weight>& router) {
			return router.BuildRoute(from, to);
		}


		std::vector<Activity> GetRouteAndBuses(std::optional<typename graph::Router<Weight>::RouteInfo>& route_info) {
			std::set<domain::Stop, transport_catalogue::StopComparer> stop_set = tc.GetStopSet();
			double wait_time = tc.GetWaitTime();
			double time = 0.0;
			int span_count = 0;
			std::vector<Activity> final_route;
			Activity go_activity;

			std::set<std::string> actual_buses;
			std::set<std::string> buses_for_route_first_stop;
			std::set<std::string> buses_for_route_second_stop;

			if (route_info.has_value()) {
				//const graph::Router<Weight>::RouteInfo& route_info_value = route_info.value();
				const auto& route_info_value = route_info.value();

				for (auto it = route_info_value.edges.begin(); it != route_info_value.edges.end(); ++it) {
					auto EdgId = *it;
					bool is_last_element = (it == std::prev(route_info_value.edges.end()));


					auto Edge = graph.GetEdge(EdgId);


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
		DirectedWeightedGraph<Weight>& graph;
		transport_catalogue::TransportCatalogue& tc;

		//std::unordered_map< Edge<double>, size_t > added_edges_;

		std::vector<Edge<Weight>> added_edges_;
		//std::vector<Edge<double>> added_edges_;
		//std::map< Edge<Weight>, size_t > added_edges_;

	};
}
