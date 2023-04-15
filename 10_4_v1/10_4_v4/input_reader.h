#pragma once
#include <set>
#include <iostream>
#include <deque>
#include <vector>
#include "transport_catalogue.h"
#include "geo.h"

namespace transport_catalogue {


	class InputReader {
	public:
		explicit InputReader(std::istream& is);

		int GetNumUpdateQueries();

		void FillRequests();

		void UpdStop(TransportCatalogue& tc);

		void UpdBus(TransportCatalogue& tc);

		void UpdStopDist(TransportCatalogue& tc);



	private:
		std::istream& is_;
		std::deque<BusDescription> upd_req_bus_;
		std::deque<Stop> upd_req_stop_;
		int num_update_q_;
		std::vector<StopDistancesDescription> distances_;
	};

}
