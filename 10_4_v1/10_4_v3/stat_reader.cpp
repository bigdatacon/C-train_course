#include <iostream>
#include "stat_reader.h"
#include "transport_catalogue.h"
using namespace std;
namespace transport_catalogue {

	int StatReader::GetNumOutQueries() {
		string line;
		getline(is_, line);
		if (line.size() != 0) {
			num_update_q_ = std::stoi(line);
			return num_update_q_;
		}
		else { return 0; }
	}

	void StatReader::FillRequests() {

		for (int i = 0; i < num_update_q_; ++i) {
			OutputRequest r;
			string line;
			getline(is_, line);
			auto space_colon = line.find(" ");
			r.name = line.substr(0, space_colon);
			r.request = line.substr(space_colon + 1);
			req_.push_back(r);
		}
	}
	void StatReader::Output(TransportCatalogue& tc) {

		for (const auto& element : req_) {
			if (element.name == "Bus"s) {
				AllBusInfoBusResponse r = tc.GetAllBusInfo(element.request);
				if (r.quant_stops == 0) {
					cout << "Bus " << element.request << ": "s << "not found" << endl;
				}
				else {
					cout << "Bus " << r.bus_name << ": "s << r.quant_stops << " stops on route, "s << r.quant_uniq_stops << " unique stops, "s << r.route_length << " route length, "s << r.route_curvature << " curvature"s << endl;
				}
			}

			if (element.name == "Stop"s) {
				const Stop* myStop = tc.FindStop(element.request);
				if (myStop == nullptr) {
					cout << "Stop " << element.request << ": not found" << endl;

				}
				else {
					set<string> r = tc.GetStopInfo(element.request);
					if (r.size() == 0) {
						cout << "Stop " << element.request << ": no buses" << endl;
					}
					else {
						cout << "Stop " << element.request << ": "s << "buses ";
						for (const auto& el : r) { cout << el << " "; }
						cout << endl;
					}
				}
			}




		}

	}

}