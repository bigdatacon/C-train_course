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

	

	transport_catalogue::TransportCatalogue tc;
	transport_catalogue::InputReaderJson reader(std::cin);
	(void)reader.ReadInputJsonRequest();

	reader.UpdStop(tc);
	reader.UpdBus(tc);
	reader.UpdStopDist(tc);


	Render_data rd = reader.GetRenderData();

	MapRenderer mapdrawer(rd);

	//RequestHandler reqhandleq(tc, mapdrawer);

	reader.ManageOutputRequests(tc, mapdrawer);

	//reqhandleq.RenderMap();


	return 0;


}
