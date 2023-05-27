#pragma once
#include "ranges.h"
#include "router.h"
#include "graph.h"
#include "transport_catalogue.h"
#include "iostream"

template <typename Weight>
graph::Router<Weight> ProcessRoute(graph::Router<Weight>& router,  transport_catalogue::TransportCatalogue tc) {
	std::cout << "start";

	DirectedWeightedGraph<int> graph(2);  // Создаем граф с 2 вершинами
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

