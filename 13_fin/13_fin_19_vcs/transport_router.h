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


		void AddStopsRoundTrip(std::deque<std::string_view> stops, std::set<domain::Stop, transport_catalogue::StopComparer> stop_set) {

			for (auto it = stops.begin(); it != std::prev(stops.end()); ++it) {

				if (it == std::prev(stops.end())) {
					size_t num_vertex_1_wait = tc.GetStopVertexIdByName(stop_set, std::string(*it)) * 2;
					size_t num_vertex_2_go = tc.GetStopVertexIdByName(stop_set, std::string(*(std::next(it)))) * 2 + 1;
					size_t num_vertex1_go = num_vertex_1_wait + 1;

					// нахожу расстояние для пары  остановок
					const Stop* stop_1 = tc.FindStop(*it);
					const Stop* stop_2 = tc.FindStop(*(std::next(it)));

					int distance_1_2 = tc.GetStopDistance(*stop_1, *stop_2);
					double time_min_1_2 = distance_1_2 / (tc.GetVelocity() * 1000 / 60);

					Edge<double> edge1 = { num_vertex_1_wait, num_vertex1_go, tc.GetWaitTime() };
					Edge<double> edge2 = { num_vertex1_go, num_vertex_2_go, time_min_1_2 };
					// Проверка наличия элемента
					if (!containsElement(added_edges_, edge1)) {
						graph.AddEdge(edge1);
						added_edges_.push_back(edge1);
					}

					if (!containsElement(added_edges_, edge2)) {
						graph.AddEdge(edge2);
						added_edges_.push_back(edge2);
					}

				}
				else {

					size_t num_vertex_1_wait = tc.GetStopVertexIdByName(stop_set, std::string(*it)) * 2;
					size_t num_vertex_2_wait = tc.GetStopVertexIdByName(stop_set, std::string(*(std::next(it)))) * 2;
					size_t num_vertex1_go = num_vertex_1_wait + 1;

					// нахожу расстояние для пары  остановок
					const Stop* stop_1 = tc.FindStop(*it);
					const Stop* stop_2 = tc.FindStop(*(std::next(it)));

					int distance_1_2 = tc.GetStopDistance(*stop_1, *stop_2);
					double time_min_1_2 = distance_1_2 / (tc.GetVelocity() * 1000 / 60);

					Edge<double> edge1 = { num_vertex_1_wait, num_vertex1_go, tc.GetWaitTime() };
					Edge<double> edge2 = { num_vertex1_go, num_vertex_2_wait, time_min_1_2 };
					// Проверка наличия элемента
					if (!containsElement(added_edges_, edge1)) {
						graph.AddEdge(edge1);
						added_edges_.push_back(edge1);
					}

					if (!containsElement(added_edges_, edge2)) {
						graph.AddEdge(edge2);
						added_edges_.push_back(edge2);
					}
				}




			}
		}

		void AddStopsNonRoundTrip(std::deque<std::string_view> stops, std::set<domain::Stop, transport_catalogue::StopComparer> stop_set) {

			for (auto it = stops.begin(); it != std::prev(stops.end()); ++it) {

				size_t num_vertex_1_wait = tc.GetStopVertexIdByName(stop_set, std::string(*it)) * 2;
				size_t num_vertex_2_wait = tc.GetStopVertexIdByName(stop_set, std::string(*(std::next(it)))) * 2;
				size_t num_vertex1_go = num_vertex_1_wait + 1;

				// нахожу расстояние для пары  остановок
				const Stop* stop_1 = tc.FindStop(*it);
				const Stop* stop_2 = tc.FindStop(*(std::next(it)));

				int distance_1_2 = tc.GetStopDistance(*stop_1, *stop_2);
				double time_min_1_2 = distance_1_2 / (tc.GetVelocity() * 1000 / 60);

				Edge<double> edge1 = { num_vertex_1_wait, num_vertex1_go, tc.GetWaitTime() };
				Edge<double> edge2 = { num_vertex1_go, num_vertex_2_wait, time_min_1_2 };

				// Проверка наличия элемента
				if (!containsElement(added_edges_, edge1)) {
					graph.AddEdge(edge1);
					added_edges_.push_back(edge1);
				}

				if (!containsElement(added_edges_, edge2)) {
					graph.AddEdge(edge2);
					added_edges_.push_back(edge2);
				}

			}

			// заполняю остановки в обратном направлении 
			for (auto it = stops.rbegin() + 1; it != stops.rend(); ++it) {

				size_t num_vertex_1_wait = tc.GetStopVertexIdByName(stop_set, std::string(*(it - 1) )) * 2;
				size_t num_vertex_2_wait = tc.GetStopVertexIdByName(stop_set, std::string(*it)) * 2;
				size_t num_vertex1_go = num_vertex_1_wait + 1;

				// нахожу расстояние для пары  остановок
				const Stop* stop_1 = tc.FindStop(*(it - 1));
				const Stop* stop_2 = tc.FindStop(*it);

				int distance_1_2 = tc.GetStopDistance(*stop_1, *stop_2);
				double time_min_1_2 = distance_1_2 / (tc.GetVelocity() * 1000 / 60);

				Edge<double> edge1 = { num_vertex_1_wait, num_vertex1_go, tc.GetWaitTime() };
				Edge<double> edge2 = { num_vertex1_go, num_vertex_2_wait, time_min_1_2 };

				// Проверка наличия элемента
				if (!containsElement(added_edges_, edge1)) {
					graph.AddEdge(edge1);
					added_edges_.push_back(edge1);
				}

				if (!containsElement(added_edges_, edge2)) {
					graph.AddEdge(edge2);
					added_edges_.push_back(edge2);
				}


			}

		}



		void AddKnots() {
			std::deque<domain::Bus> buses_ = tc.GetBuses();
			std::set<domain::Stop, transport_catalogue::StopComparer> stop_set = tc.GetStopSet();


			for (Bus bus : buses_) {
				std::string type = bus.type;
				std::deque<std::string_view> stops = bus.stops;

				if (type == "true") {

					AddStopsRoundTrip(stops, stop_set);
				}
				else {

					AddStopsNonRoundTrip(stops, stop_set);
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
					std::string first_stop_name = tc.GetStopNameByVertexId(stop_set, Edge.from);
					std::string second_stop_name = tc.GetStopNameByVertexId(stop_set, Edge.to);

					if (first_stop_name == second_stop_name) {
						if (!IsEmptyActivity(go_activity) && !actual_buses.empty()) {
							go_activity.time = time;
							go_activity.span_count = span_count;
							go_activity.stop_name_to = second_stop_name;
							go_activity.type_activity = "Bus";
							go_activity.bus_name = *actual_buses.begin();
							final_route.push_back(go_activity);
							ClearActivity(go_activity);
							time = 0.0;
							span_count = 0;
							actual_buses.clear();
						}

						Activity activity;
						activity.type_activity = "Wait";
						activity.time = wait_time;
						activity.stop_name_from = first_stop_name;
						activity.stop_name_to = second_stop_name;
						activity.span_count = 0;
						final_route.push_back(activity);

					}
					else {
						if (!actual_buses.empty()) {
							buses_for_route_first_stop = tc.GetStopInfo(first_stop_name);
							buses_for_route_second_stop = tc.GetStopInfo(second_stop_name);
							std::set<std::string> actual_buses_0_1 = GetCommonElements(actual_buses, buses_for_route_first_stop);
							actual_buses = GetCommonElements(actual_buses_0_1, buses_for_route_second_stop);
							time += Edge.weight;
							span_count += 1;
						}
						else {

							buses_for_route_first_stop = tc.GetStopInfo(first_stop_name);
							buses_for_route_second_stop = tc.GetStopInfo(second_stop_name);
							actual_buses = GetCommonElements(buses_for_route_first_stop, buses_for_route_second_stop);
							time += Edge.weight;
							span_count += 1;
							go_activity.stop_name_from = first_stop_name;
						}
					}
					if (is_last_element && !actual_buses.empty() && !IsEmptyActivity(go_activity)) {
						go_activity.time = time;
						go_activity.span_count = span_count;
						go_activity.stop_name_to = second_stop_name;
						go_activity.type_activity = "Bus";
						go_activity.bus_name = *actual_buses.begin();
						final_route.push_back(go_activity);
					}
				}

				return final_route;
			}
			else {
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

