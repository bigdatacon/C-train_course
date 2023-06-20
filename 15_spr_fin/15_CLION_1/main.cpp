/*
#include <iostream>

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
*/

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

#include "transport_router.h"


int main() {
#ifdef _DEBUG
    if (freopen("input_in.txt.txt", "r", stdin) == nullptr) {
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
    reader.UpdSerializeSettings(tc);

    std::cout << "Finish"<< std::endl;

    return 0;


}