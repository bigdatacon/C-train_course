#pragma once
#include "ranges.h"
#include "router.h"
#include "graph.h"
#include "transport_catalogue.h"
#include <iostream>

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

		//template <typename Weight>
		void AddKnots() {
			auto stops_distance_time_ = tc.GetstopsDistanceTime();
			std::set<domain::Stop, transport_catalogue::StopComparer> stop_set = tc.GetStopSet();
			for (auto it = stops_distance_time_.begin(); it != stops_distance_time_.end(); ++it) {
				size_t num_vertex_1 = tc.GetStopVertexIdByName(stop_set, it->first.first->stop_name)*2;
				size_t num_vertex_2 = tc.GetStopVertexIdByName(stop_set, it->first.second->stop_name)*2;

				/*с учетом вот этого услови€ : "Ќе более 100 остановок в маршруте.  оличество запросов на создание автобусов и остановок Ч не более 100"*/


				//size_t num_vertex_wait = num_vertex_1 + 40000 ;
				size_t num_vertex_wait = num_vertex_1+1;
				//std::cout << "it->second " << it->second << std::endl;

				//Edge<double> edge1 = { num_vertex_1, num_vertex_1, tc.GetWaitTime() };
				Edge<double> edge1 = { num_vertex_1, num_vertex_wait, tc.GetWaitTime() };
				Edge<double> edge2 = { num_vertex_1, num_vertex_2, it->second };
				graph.AddEdge(edge1);
				graph.AddEdge(edge2);
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

		/*
		//template <typename Weight>
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
				const graph::Router<Weight>::RouteInfo& route_info_value = route_info.value();

				for (auto it = route_info_value.edges.begin(); it != route_info_value.edges.end(); ++it) {
					auto EdgId = *it;
					bool is_last_element = (it == std::prev(route_info_value.edges.end()));


				//for (size_t EdgId : route_info_value.edges) {

					auto Edge = graph.GetEdge(EdgId);
					std::string first_stop_name = tc.GetStopNameByVertexId(stop_set, Edge.from);
					std::string second_stop_name = tc.GetStopNameByVertexId(stop_set, Edge.to);

					//std::cout << "Edge.weight" << Edge.weight<< std::endl;

					if (Edge.from == Edge.to) {
						if (!IsEmptyActivity(go_activity)) {
							go_activity.time = time;
							go_activity.span_count = span_count;
							go_activity.stop_name_to = second_stop_name;
							go_activity.type_activity = "Bus";
							go_activity.bus_name = *actual_buses.begin();
							final_route.push_back(go_activity);
							ClearActivity(go_activity);
							time = 0.0;
							span_count = 0;
						}

						Activity activity;
						activity.type_activity = "Wait";
						activity.time = wait_time;
						activity.stop_name_from = first_stop_name;
						activity.stop_name_to = second_stop_name;
						activity.span_count = 0;
						final_route.push_back(activity);

						if (!actual_buses.empty()) {
							actual_buses.clear();
						}
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
							if (span_count == 0) {
								Activity activity;
								activity.type_activity = "Wait";
								activity.time = wait_time;
								activity.stop_name_from = first_stop_name;
								activity.stop_name_to = second_stop_name;
								activity.span_count = 0;
								final_route.push_back(activity);
							}



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
		}
		*/


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
				const graph::Router<Weight>::RouteInfo& route_info_value = route_info.value();

				for (auto it = route_info_value.edges.begin(); it != route_info_value.edges.end(); ++it) {
					auto EdgId = *it;
					bool is_last_element = (it == std::prev(route_info_value.edges.end()));


					//for (size_t EdgId : route_info_value.edges) {

					auto Edge = graph.GetEdge(EdgId);
					std::string first_stop_name = tc.GetStopNameByVertexId(stop_set, Edge.from);
					std::string second_stop_name = tc.GetStopNameByVertexId(stop_set, Edge.to);

					//std::cout << "Edge.weight" << Edge.weight<< std::endl;

					if (Edge.from == Edge.to) {
						if (!IsEmptyActivity(go_activity)) {
							go_activity.time = time;
							go_activity.span_count = span_count;
							go_activity.stop_name_to = second_stop_name;
							go_activity.type_activity = "Bus";
							go_activity.bus_name = *actual_buses.begin();
							final_route.push_back(go_activity);
							ClearActivity(go_activity);
							time = 0.0;
							span_count = 0;
						}

						Activity activity;
						activity.type_activity = "Wait";
						activity.time = wait_time;
						activity.stop_name_from = first_stop_name;
						activity.stop_name_to = second_stop_name;
						activity.span_count = 0;
						final_route.push_back(activity);

						if (!actual_buses.empty()) {
							actual_buses.clear();
						}
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
							if (span_count == 0) {
								Activity activity;
								activity.type_activity = "Wait";
								activity.time = wait_time;
								activity.stop_name_from = first_stop_name;
								activity.stop_name_to = second_stop_name;
								activity.span_count = 0;
								final_route.push_back(activity);
							}



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
		}




			//template <typename Weight>
			//DirectedWeightedGraph<Weight> GetGraph() { return graph; }

			//template <typename Weight>
			//graph::Router<Weight> GetRouter() { return router_; }


	private:
		DirectedWeightedGraph<Weight>& graph;
		transport_catalogue::TransportCatalogue& tc;
		//graph::Router<Weight> router_;


		};
	}


