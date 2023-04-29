#define _CRT_SECURE_NO_WARNINGS
#include "geo.h"
#include "transport_catalogue.h"
#include "json_reader.h"
#include "iostream"
#include "map_renderer.h"
#include "request_handler.h"

using namespace transport_catalogue;
using namespace std;
#include <chrono>


int main() {
#ifdef _DEBUG
	if (freopen("input.txt.txt", "r", stdin) == nullptr) {
		puts("can't open input.txt.txt");
		return 1;
	}
#endif 


	//I через класс 


	transport_catalogue::TransportCatalogue tc;
	transport_catalogue::InputReaderJson reader(std::cin);
	(void)reader.ReadInputJsonRequest();

	reader.UpdStop(tc);
	reader.UpdBus(tc);
	reader.UpdStopDist(tc);


	Render_data rd = reader.GetRenderData();

	MapRenderer mapdrawer(rd);

	RequestHandler reqhandleq(tc, mapdrawer);


	reqhandleq.RenderMap();



	
	
	//II без класса 

	/*
	auto start_time = std::chrono::high_resolution_clock::now(); // начальное время
	transport_catalogue::TransportCatalogue tc;
	transport_catalogue::InputReaderJson reader(std::cin);
	(void)reader.ReadInputJsonRequest();

	reader.UpdStop(tc);
	reader.UpdBus(tc);
	reader.UpdStopDist(tc);

	//reader.ManageOutputRequests(tc);

	Render_data rd = reader.GetRenderData();
	//DrawRoute(rtotc, rd);   
	DrawRoute(tc, rd);

	auto end_time = std::chrono::high_resolution_clock::now(); // конечное время
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time); // время выполнения в микросекундах
	std::cout << "TIME PASSED : " << duration.count() << " MILISECOND" << std::endl;
	*/
	return 0;


}
