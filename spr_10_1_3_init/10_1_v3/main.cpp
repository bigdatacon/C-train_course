#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <deque>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <math.h>
#include <regex>
#include <cmath>
#include <unordered_set>
#include "stat_reader.h"
#include "geo.h"
#include "transport_catalogue.h"
#include "input_reader.h"

using namespace std;




int main()
{
	TransportCatalogue tc;
    InputReader reader(std::cin);
    int count = reader.GetNumUpdateQueries();
    //std::cout << "COUNT : " << count << std::endl;
	reader.FillRequests();

	reader.GetUpdStop(tc);
	reader.GetUpdBus(tc);

	string b = "750"s;

	tc.GetAllBusInfo(b);

	StatReader streader(std::cin);
	int count_out = streader.GetNumOutQueries();
	streader.FillRequests();
	streader.Output(tc);
	



	// как говорили на вэбинаре для класса out не нужно записывать запросы в контейнер поэтому делаю обычный вывод из cin
	for (int i = 0; i < count_out; ++i) {
		string line;
		getline(cin, line);
		auto space_colon = line.find(" ");  // : отделяет название запроса
		string str = line.substr(space_colon + 1);
		cout << tc.GetAllBusInfo(str) << endl;
	}

	
    return 0;
}
