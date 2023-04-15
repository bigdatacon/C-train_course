#pragma once

#include <iostream>
#include <string>
#include <set>
#include <algorithm>
#include <deque>

#include "geo.h"
#include "transport_catalogue.h"
namespace transport_catalogue {

	struct OutputRequest {
		std::string name;
		std::string request;
	};

	class StatReader {
	public:
		explicit StatReader(std::istream& is) : is_(is) {};


		int GetNumOutQueries();

		void FillRequests();
		void Output(TransportCatalogue& tc);

	private:
		std::istream& is_;
		std::deque<OutputRequest> req_;
		int num_update_q_;
	};

}
