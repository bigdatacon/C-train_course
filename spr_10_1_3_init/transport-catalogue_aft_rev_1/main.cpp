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
	project_namespace::TransportCatalogue tc;
    project_namespace::InputReader reader(std::cin);
     (void) reader.GetNumUpdateQueries();
	reader.FillRequests();

	reader.GetUpdStop(tc);
	reader.GetUpdBus(tc);
    reader.GetUpdStopDist(tc);

	project_namespace::StatReader streader(std::cin);
	(void)  streader.GetNumOutQueries();
	streader.FillRequests();
	streader.Output(tc);

    return 0;
}
