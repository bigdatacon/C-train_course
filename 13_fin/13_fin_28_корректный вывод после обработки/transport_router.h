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


		void AddStopsOneDirection(std::deque<std::string_view> stops, std::set<domain::Stop, transport_catalogue::StopComparer> stop_set) {

			for (auto it = stops.begin(); std::next(it) != stops.end(); ++it) {
				double sum_time = 0; // накапливаемое время движения по ходу маршрута с каждой следующей остановкой сюда 
				size_t num_vertex_1_wait = tc.GetStopVertexIdByName(stop_set, std::string(*it)) * 2;
				size_t num_vertex1_go = num_vertex_1_wait + 1;
				if (it == std::prev(stops.end())) { // проверяю что дошел до последней остановки и заканчиваю цикл 

					Edge<double> edge_mirror_next = { num_vertex_1_wait, num_vertex1_go ,   tc.GetWaitTime() };  // для последней остановки добавляю только wait 
					if (!containsElement(added_edges_, edge_mirror_next)) {
						graph.AddEdge(edge_mirror_next);
						added_edges_.push_back(edge_mirror_next);
					}


					break;

				}


				size_t num_vertex_next_wait = tc.GetStopVertexIdByName(stop_set, std::string(*std::next(it))) * 2;
				size_t num_vertex_next_go = num_vertex_next_wait + 1;

				const Stop* stop_1 = tc.FindStop(*it);
				const Stop* stop_1_next = tc.FindStop(*std::next(it));
				int distance_inner = tc.GetStopDistance(*stop_1, *stop_1_next); // расстояние от остановки 
				double time_inner = distance_inner / (tc.GetVelocity() * 1000 / 60) + sum_time;

				/*if (std::next(it) == std::prev(stops.end())) { // проверяю что дошел до предпоследней остановки и заканчиваю цикл

					Edge<double> edge_mirror_next = { num_vertex_1_wait, num_vertex1_go ,   tc.GetWaitTime() };  // для последней остановки добавляю только wait
					if (!containsElement(added_edges_, edge_mirror_next)) {
						graph.AddEdge(edge_mirror_next);
						added_edges_.push_back(edge_mirror_next);
					}


					//Edge<double> edge_go = { num_vertex1_go, num_vertex_next_go, time_inner };  // еду до ожидания
					Edge<double> edge_go = { num_vertex1_go, num_vertex_next_wait, time_inner };  // еду до ожидания
					if (!containsElement(added_edges_, edge_go)) {
						graph.AddEdge(edge_go);
						added_edges_.push_back(edge_go);
					}

				}
				else {*/

				Edge<double> edge_mirror = { num_vertex_1_wait, num_vertex1_go, tc.GetWaitTime() };  // добавляю ребро зеркало для первой остановки 
				//Edge<double> edge_go = { num_vertex1_go, num_vertex_next_go, time_inner };  // добавляю ребро зеркало для первой остановки 
				Edge<double> edge_go = { num_vertex1_go, num_vertex_next_wait, time_inner };  // добавляю ребро зеркало для первой остановки 

				if (!containsElement(added_edges_, edge_mirror)) {
					graph.AddEdge(edge_mirror);
					added_edges_.push_back(edge_mirror);
				}
				if (!containsElement(added_edges_, edge_go)) {
					graph.AddEdge(edge_go);
					added_edges_.push_back(edge_go);
				}
				sum_time = time_inner;

				//}

				/*

				// ребра на "ЕХАТЬ из останвовки во внешнем цикле добавяю во внутреннем"
				for (auto it_inner = std::next(it); std::next(it_inner) != stops.end(); ++it_inner){
				//for (auto it_inner = stops.begin()+1; std::next(it_inner)!= stops.end(); ++it_inner) {  // иду до предпоследней останвокий так как вторая в паре остановка - последняя в такой конструкции вроде как

					// тут уже (это значит минимум через одну остановку от той что во внешнем цикле - ищу растояние между сосединими и к нему прибавляю расстояние (то есть время) накопленное пока ехал до этой пары остановок а именно до 1)

					//size_t num_vertex_inner_wait = tc.GetStopVertexIdByName(stop_set, std::string(*it_inner)) * 2;
					//size_t num_vertex_inner_go = num_vertex_inner_wait + 1;

					size_t num_vertex_inner_next_wait = tc.GetStopVertexIdByName(stop_set, std::string(*std::next(it_inner))) * 2;
					size_t num_vertex_inner_next_go = num_vertex_inner_next_wait + 1;

					const Stop* stop_inner = tc.FindStop(*it_inner);
					const Stop* stop_inner_next = tc.FindStop(*(std::next(it_inner)));
					int distance_1_2 = tc.GetStopDistance(*stop_inner, *stop_inner_next); // расстояние от остановки


					double time_min_1_2 = distance_1_2 / (tc.GetVelocity() * 1000 / 60) + sum_time; // увиличиваю время на то что уже было накоплено пока ехал до этой остановки
					//std::cout << "Stop1 , Stop 2" << std::string(*it_inner) << " " << *it << " time_min_1_2 " << time_min_1_2 << std::endl;

					if (std::next(it_inner) == std::prev(stops.end())) { // проверяю что следующ остановка последняя, значит в последнюю едем в зеракало то есть в wait но тут не уверен нужно prev(stops.end() или просто stops.end() !!!!????

						Edge<double> edge_mirror_inner = { num_vertex1_go, num_vertex_inner_next_wait , time_min_1_2 };
						// Проверка наличия элемента

						if (!containsElement(added_edges_, edge_mirror_inner)) {
							graph.AddEdge(edge_mirror_inner);
							added_edges_.push_back(edge_mirror_inner);
						}

					}

					else {
						Edge<double> edge_go_inner = { num_vertex1_go, num_vertex_inner_next_go , time_min_1_2 };
						// Проверка наличия элемента

						if (!containsElement(added_edges_, edge_go_inner)) {
							graph.AddEdge(edge_go_inner);
							added_edges_.push_back(edge_go_inner);
						}
					}

					sum_time = time_min_1_2; // присваиваю значение накопленного времени - текущее потраченное время

				}
				*/
			}

		}



		void AddStopsNonRoundTrip(std::deque<std::string_view> stops, std::set<domain::Stop, transport_catalogue::StopComparer> stop_set) {
			AddStopsOneDirection(stops, stop_set); // Заполняю в прямом направлении
			std::cout << "BEFORE" << std::endl;
			for (auto s : stops) { std::cout << std::string(s) << std::endl; }


			std::reverse(stops.begin(), stops.end()); // разворачияю списов остановок 
			std::cout << "AFTER" << std::endl;
			for (auto s : stops) { std::cout << std::string(s) << std::endl; }
			AddStopsOneDirection(stops, stop_set); // Заполняю в обратном направлении

		}

		void AddKnots() {
			std::deque<domain::Bus> buses_ = tc.GetBuses();
			std::set<domain::Stop, transport_catalogue::StopComparer> stop_set = tc.GetStopSet();


			for (Bus bus : buses_) {
				std::string type = bus.type;
				std::deque<std::string_view> stops = bus.stops;

				if (type == "true") {

					AddStopsOneDirection(stops, stop_set);
				}
				else {
					std::cout << "Non_round_TRIP" << std::endl;

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

		/*
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
				bool first = true;
				for (auto it = route_info_value.edges.begin(); it != route_info_value.edges.end(); ++it) {
					auto EdgId = *it;
					bool is_last_element = (it == std::prev(route_info_value.edges.end()));


					auto Edge = graph.GetEdge(EdgId);
					std::string first_stop_name = tc.GetStopNameByVertexId(stop_set, Edge.from);
					std::string second_stop_name = tc.GetStopNameByVertexId(stop_set, Edge.to);

					if (first_stop_name == second_stop_name) {
						if (first )  {

							Activity activity;
							activity.type_activity = "Wait";
							activity.time = wait_time;
							activity.stop_name_from = first_stop_name;
							activity.stop_name_to = second_stop_name;
							activity.span_count = 0;
							final_route.push_back(activity);
							first = false;

						}


						if (!actual_buses.empty()) {

							buses_for_route_first_stop = tc.GetStopInfo(first_stop_name);
							actual_buses = GetCommonElements(actual_buses, buses_for_route_first_stop);

							if (actual_buses.size()!=0) {
								continue; // не добавляю ожидание так как могу ехать на том же  на том же автобусе

							}
							else {
								go_activity.time = time;
								go_activity.span_count = span_count;
								go_activity.stop_name_to = first_stop_name;
								go_activity.type_activity = "Bus";
								go_activity.bus_name = *buses_for_route_first_stop.begin();
								final_route.push_back(go_activity);
								ClearActivity(go_activity);
								time = 0.0;
								span_count = 0;
								actual_buses.clear();

								Activity activity;
								activity.type_activity = "Wait";
								activity.time = wait_time;
								activity.stop_name_from = first_stop_name;
								activity.stop_name_to = second_stop_name;
								activity.span_count = 0;
								final_route.push_back(activity);
							}



						}






					}
					else {
						if (!actual_buses.empty()) {
							buses_for_route_first_stop = tc.GetStopInfo(first_stop_name);
							buses_for_route_second_stop = tc.GetStopInfo(second_stop_name);
							std::set<std::string> actual_buses_0_1 = GetCommonElements(actual_buses, buses_for_route_first_stop);
							actual_buses = GetCommonElements(actual_buses_0_1, actual_buses);
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
		*/


		/*
		std::vector<Activity> ProcessFinalRoute(std::vector<Activity>& final_route) {
			std::vector<Activity> wait;
			std::vector<Activity> go;
			std::vector<Activity> itg;

			for (Activity activity : final_route) {
				if (activity.type_activity == "Bus") {
					go.push_back(activity);
				else { wait.push_back(activity); }

				for (auto it_inner = go.begin(); std::next(it_inner) != go.end(); ++it_inner) {
					if (*it_inner.bus_name == *(std::next(it_inner)).bus_name) {
						continue;

					}
					else {
						wait.erase(wait.begin() + std::distance(go.begin() + std::next(it_inner)));

					}

				}

		}
				*/

		std::vector<Activity> ProcessFinalRoute(std::vector<Activity>& final_route) {
			std::vector<Activity> real_final_route;
			
			for (size_t i = 1; i < final_route.size() - 1; ++i) {
				const Activity& current = final_route[i];

				//if (i+1  == final_route.size() && current.type_activity == "Wait") { final_route.erase(final_route.begin() + i); }

				if (final_route.back().type_activity == "Wait") {
					final_route.pop_back();
				}

				if (i + 1 < final_route.size()) {
					const Activity& next = final_route[i + 1];
					const Activity& prev = final_route[i - 1];

					if (current.type_activity == "Wait") {

						if (next.bus_name == prev.bus_name) { final_route.erase(final_route.begin() + i); }
					}

				}
			}


			std::string bus_name_f;
			int span_count_f = 0;
			double time_f = 0;
			Activity activity_f;
			bool first = true;
			for (Activity el : final_route) {
				if (el.type_activity == "Wait") { 
					if (first) {
						real_final_route.push_back(el);
						first = false;
					}
					else {
						
						activity_f.span_count = span_count_f;
						activity_f.time = time_f;
						real_final_route.push_back(activity_f);
						span_count_f = 0;
						time_f = 0;
						real_final_route.push_back(el);
					}
				}
				else {
					span_count_f += 1;
					time_f += el.time;
					//bus_name_f = el.bus_name;
					activity_f = el;
				}



			}
			activity_f.span_count = span_count_f;
			activity_f.time = time_f;
			real_final_route.push_back(activity_f);

			//return final_route;
			return real_final_route;
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
							actual_buses = GetCommonElements(actual_buses_0_1, actual_buses);
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



				final_route = ProcessFinalRoute(final_route);
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

