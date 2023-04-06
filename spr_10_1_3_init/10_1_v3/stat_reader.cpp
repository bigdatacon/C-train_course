#include "stat_reader.h"
#include "transport_catalogue.h"
using namespace std;

ostream& operator<<(ostream& os, const AllBusInfoBusResponse& r) {
	if (r.stops == 0) {
		cout << "not found bus ";

	}
	else {
		cout << "Bus " << r.bus << " :"s << r.stops << " stops on route, "s << r.uniq_stops << " unique stops, "s << r.r_length << " route length"s << endl;
	}
	return os;
}


StatReader::StatReader(istream& is) : is_(is) {
}

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
		StatR r;
		string line;
		getline(is_, line);
		auto space_colon = line.find(" ");  
		r.name = line.substr(0, space_colon);
		r.str = line.substr(space_colon + 1);
		req_.push_back(r);
	}
}
void StatReader::Output(TransportCatalogue& tc) {
	for (const auto& element : req_) {
		if (element.name == "Bus"s) {
			AllBusInfoBusResponse r = tc.GetAllBusInfo(element.str);
			if (r.stops == 0) {
				cout << "Bus " << element.str << ": "s << "not found" << endl;
			}
			else {
				cout << "Bus " << r.bus << ": "s << r.stops << " stops on route, "s << r.uniq_stops << " unique stops, "s << r.r_length << " route length"s << endl;
			}
		}
	}
}

