#include "stat_reader.h"
#include "geo.h"
#include "transport_catalogue.h"
#include "input_reader.h"

using namespace std;


int main()
{
	transport_catalogue::TransportCatalogue tc;
	transport_catalogue::InputReader reader(std::cin);
	(void)reader.GetNumUpdateQueries();
	reader.FillRequests();

	reader.UpdStop(tc);
	reader.UpdBus(tc);
	reader.UpdStopDist(tc);

	transport_catalogue::StatReader streader(std::cin);
	(void)streader.GetNumOutQueries();
	streader.FillRequests();
	streader.Output(tc);

	return 0;
}
