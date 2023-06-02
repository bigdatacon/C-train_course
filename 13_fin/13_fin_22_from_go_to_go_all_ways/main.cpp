#define _CRT_SECURE_NO_WARNINGS
#include "geo.h"
#include "transport_catalogue.h"
#include "json_reader.h"
#include "iostream"
#include "map_renderer.h"
#include "request_handler.h"
#include <fstream>
using namespace transport_catalogue;
using namespace std;
#include <chrono>

//#include "graph.h"
//#include "router.h"

#include "transport_router.h"


int main() {
#ifdef _DEBUG
	if (freopen("input.txt.txt", "r", stdin) == nullptr) {
		puts("can't open input.txt.txt");
		return 1;
	}
#endif 
	
	transport_catalogue::TransportCatalogue tc;
	transport_catalogue::InputReaderJson reader(std::cin);
	(void)reader.ReadInputJsonRequest();

	reader.UpdStop(tc);
	reader.UpdBus(tc);
	reader.UpdStopDist(tc);
	reader.UpdRouteSettings(tc);
	RenderData rd = reader.GetRenderData();
	MapRenderer mapdrawer(rd);

	//������ ������� ������� ���� � ������ � �������� ������� 
	std::set<domain::Stop, transport_catalogue::StopComparer> stop_set = tc.GetStopSet(); // ������� ���������� ���������
	graph::DirectedWeightedGraph<double> graph(stop_set.size()*2+1); // ������ ���� � 2 ���������


	graph::ActivityProcessor activityprocessor(graph, tc);
	
	graph::Router<double> router = activityprocessor.ProcessRoute();

	
	reader.ManageOutputRequests(tc, mapdrawer, activityprocessor, router);

	//reader.ManageOutputRequests(tc, mapdrawer);


	return 0;


}
