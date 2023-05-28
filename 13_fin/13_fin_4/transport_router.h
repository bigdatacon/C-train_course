#pragma once
#include "ranges.h"
#include "router.h"
#include "graph.h"
#include "transport_catalogue.h"
#include "iostream"


struct Activity {
	std::string stop_name_from;
	std::string stop_name_to;

	double time;
	std::string type_activity;
};

bool IsEmptyActivity(const Activity& activity) {
	return activity.stop_name_from.empty() &&
		activity.stop_name_to.empty() &&
		activity.time == 0.0 &&
		activity.type_activity.empty();
}

void ClearActivity(Activity& activity) {
	activity.stop_name_from.clear();
	activity.stop_name_to.clear();
	activity.time = 0.0;
	activity.type_activity.clear();
}


std::set<std::string> GetCommonElements(const std::set<std::string>& set1, const std::set<std::string>& set2) {
	std::set<std::string> result;
	std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(result, result.begin()));
	return result;
}


//DirectedWeightedGraph<int> graph(2); -- это нужно в main отдельно создать или передать 

template <typename Weight>
graph::Router<Weight> ProcessRoute(graph::DirectedWeightedGraph<Weight>& graph, graph::Router<Weight>& router, transport_catalogue::TransportCatalogue tc) {
	std::cout << "start";

	//DirectedWeightedGraph<int> graph(2);  // Создаем граф с 2 вершинами -- нужно создание графа вынести отдельно 
	graph(2);
	AddKnots(graph, tc);
	//graph::Router router(graph); // переношу граф в роутер 
	router(graph);
	return router;
}


template <typename Weight>
std::optional<typename graph::Router<Weight>::RouteInfo> GetShortestRoute(graph::Router<Weight>& router, graph::VertexId from,
	graph::VertexId to) {
	return router.BuildRoute(from, to);


}




template <typename Weight>
std::vector<Activity> GetRouteAndBuses(graph::DirectedWeightedGraph<Weight>& graph, std::optional<typename graph::Router<Weight>::RouteInfo>& route_info, transport_catalogue::TransportCatalogue tc) {
	std::set<domain::Stop, StopComparer> stop_set = tc.GetStopSet();
	double time = 0.0;

	std::vector<Activity> final_route;
	Activity go_activity;

	std::set<std::string> actual_buses;
	for (auto EdgId : route_info.edges) {
		auto Edge = graph.GetEdge(EdgId);
		std::string first_stop_name = tc.GetStopNameByVertexId(EdgId.from);
		std::string second_stop_name = tc.GetStopNameByVertexId(EdgId.to);

		if (Edge.from == Edge.to) {
			if (!IsEmptyActivity(go_activity)) {
				go_activity.time = time;
				go_activity.stop_name_to = Edge.to;
				go_activity.type_activity = 'go';
				final_route.push_back(go_activity);
				ClearActivity(go_activity);
				time = 0.0;

			}

			Activity activity;

			activity.type_activity == 'wait';
			activity.time == Edge.weight;
			activity.stop_name_from = EdgId.from;
			activity.stop_name_to = EdgId.to;
			final_route.push_back(activity);

			if (actual_buses) {
				actual_buses.clear();
			}
		}

		else {

			if (actual_buses) {
				buses_for_route_first_stop = GetStopInfo(first_stop_name);
				buses_for_route_second_stop = GetStopInfo(second_stop_name);
				std::set<std::string> actual_buses_0_1 = GetCommonElements(actual_buses, buses_for_route_first_stop);
				actual_buses = GetCommonElements(actual_buses_0_1, buses_for_route_second_stop);
				time += Edge.weight;

			}
			else {
				buses_for_route_first_stop = GetStopInfo(first_stop_name);
				buses_for_route_second_stop = GetStopInfo(second_stop_name);
				actual_buses = GetCommonElements(buses_for_route_first_stop, buses_for_route_second_stop)
				time += Edge.weight;
				go_activity.stop_name_from = EdgId.from;
			}
		}

	}
	return final_route;
}


