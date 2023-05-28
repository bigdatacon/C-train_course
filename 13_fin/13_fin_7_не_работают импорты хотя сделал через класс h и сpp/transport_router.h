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

	bool IsEmptyActivity(const Activity& activity) {
		return activity.stop_name_from.empty() &&
			activity.stop_name_to.empty() &&
			activity.time == 0.0 &&
			activity.type_activity.empty() && activity.bus_name.empty() && activity.span_count == 0;
	}

	void ClearActivity(Activity& activity) {
		activity.stop_name_from.clear();
		activity.stop_name_to.clear();
		activity.time = 0.0;
		activity.type_activity.clear();
		activity.bus_name.clear();
		activity.span_count = 0.0;
	}


	std::set<std::string> GetCommonElements(const std::set<std::string>& set1, const std::set<std::string>& set2) {
		std::set<std::string> result;
		std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(result, result.begin()));
		return result;
	}

	std::ostream& operator<<(std::ostream& os, const Activity& activity) {
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
		ActivityProcessor(DirectedWeightedGraph<Weight>& graph, transport_catalogue::TransportCatalogue& tc /*, graph::Router<Weight>& router*/);

		std::set<std::string> GetCommonElements(const std::set<std::string>& set1, const std::set<std::string>& set2);
		void AddKnots();
		graph::Router<Weight> ProcessRoute();
		std::optional<typename graph::Router<Weight>::RouteInfo> GetShortestRoute( graph::VertexId from, graph::VertexId to);
		std::vector<Activity> GetRouteAndBuses(std::optional<typename graph::Router<Weight>::RouteInfo>& route_info);

	
		DirectedWeightedGraph<Weight> GetGraph();

		graph::Router<Weight> GetRouter();


	private:
		DirectedWeightedGraph<Weight>& graph;
		transport_catalogue::TransportCatalogue& tc;
		graph::Router<Weight>& router_;


	};
}


