#include "geo.h"
#include "transport_catalogue.h"
#include "json_reader.h"

using namespace std;

/*
int main() {
	
	 * Примерная структура программы:
	 *
	 * Считать JSON из stdin
	 * Построить на его основе JSON базу данных транспортного справочника
	 * Выполнить запросы к справочнику, находящиеся в массиве "stat_requests", построив JSON-массив
	 * с ответами.
	 * Вывести в stdout ответы в виде JSON
	 
}
*/

int main()
{
	transport_catalogue::TransportCatalogue tc;
	transport_catalogue::InputReaderJson reader(std::cin);
	(void)reader.ReadInputJsonRequest();
	(void)reader.ReadOutputJsonRequest();

	reader.UpdStop(tc);
	reader.UpdBus(tc);
	reader.UpdStopDist(tc);

	
	
	/*transport_catalogue::StatReader streader(std::cin);
	(void)streader.GetNumOutQueries();
	streader.FillRequests();
	streader.Output(tc);*/

	return 0;
}

