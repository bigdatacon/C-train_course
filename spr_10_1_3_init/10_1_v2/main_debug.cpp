

inline double ComputeDistance(Coordinates from, Coordinates to) {
	using namespace std;
	if (from == to) {
		return 0;
	}
	static const double dr = 3.1415926535 / 180.;
	return acos(sin(from.lat * dr) * sin(to.lat * dr)
		+ cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
		* 6371000;
}

struct Coordinates {
	double lat;
	double lng;
	bool operator==(const Coordinates& other) const {
		return lat == other.lat && lng == other.lng;
	}
	bool operator!=(const Coordinates& other) const {
		return !(*this == other);
	}
};

struct Stop {
	string stop;
	Coordinates coordinates;
};

struct Bus {
	string bus;
	vector<string> stops;
	string type;
};

vector<string> SplitStringBySignPlain(string str) {
	vector<string> tokens;
	stringstream ss(str);
	string token;
	while (getline(ss, token, '-'))
	{
		tokens.push_back(token);
	}
	return tokens;
}

vector<string> SplitStringBySignCircle(string str) {
	vector<string> tokens;
	stringstream ss(str);
	string token;
	while (getline(ss, token, '>'))
	{
		tokens.push_back(token);
	}
	int half_size = tokens.size() / 2;
	vector<string>::iterator first = tokens.begin();
	vector<string>::iterator last = tokens.begin() + half_size;
	vector<string> half_vec(first, last);
	return half_vec;
}



pair<double, double> SplitStringByComma(string str) {
	pair<double, double> coordinates;
	stringstream ss(str);
	string token;

	getline(ss, token, ',');
	coordinates.first = stod(token);

	getline(ss, token, ',');
	coordinates.second = stod(token);

	return coordinates;
}
struct Busptr {
	string bus;
	deque<string*> stops;
	string type;
};

class TransportCatalogue {
public:
	void AddBus(const Bus b) {
		Busptr bptr;
		deque <string*> stops_ptr;
		// тут нужно в buses добавить вместо string названий остановок - ссылки на остановки 
		for (auto stop : b.stops) {
			stop_name_to_stop_.find(stop)
			auto it = stop_name_to_stop_.find(stop);
			if (it != stop_name_to_stop_.end()) {
				Stop* stop_ptr = it->second;
				stops_ptr.push_back(it->second);
			}
		
		}
		bptr.bus = b.bus;
		bptr.type = b.type;  // sv
		bptr.stops = stops_ptr;
		buses_.push_back(bptr); //move
		Busptr* bptr_bus = &bptr;
		bus_name_to_bus_.emplace(b.bus, bptr_bus);

	}

	void AddStop(const Stop s) {
		Stop* ptr_stop = &s;
		stop_name_to_stop_.emplace(s.stop, ptr_stop);
		stops_.push_back(s); //move
	}

	Busptr FindBus(const string bus) {
		Busptr res;
		if (bus_name_to_bus_.count(bus)) { return bus_name_to_bus_[bus]; }
		else { cout << "Bus " << bus << ": not found" << endl; return res; }
	}

	Stop FindStop(const string stop) {
		Stop res;
		if (stop_name_to_stop_.count(stop_new)) { return stop_name_to_stop_[stop]; }
		else { cout << "Stop " << stop << ": not found" << endl; return res; }
	}

	AllBusInfoBusResponse GetAllBusInfo(const string bus) {
		AllBusInfoBusResponse all_r;
		Busptr fb = FindBus(bus);
		deque<string*> stops_v = fb.stops;
		if (stops_v.size() != 0) {
			all_r.bus = bus;
			if (fb.type == "c"s) {
				all_r.stops = stops_v.size() * 2;
				all_r.uniq_stops = stops_v.size();
			}
			else {
				all_r.stops = stops_v.size() * 2 - 1;
				all_r.uniq_stops = stops_v.size();
			}
			int lap = 0;
			for (int i = 0; i < stops_v.size() - 1; i++) {
				pair<double, double> one = FindStop(*stops_v[i]).coordinates;
				pair<double, double> two = FindStop(*stops_v[i + 1]).coordinates;
				++lap;
				Coordinates c_one;
				Coordinates c_two;
				c_one.lat = one.first;
				c_one.lng = one.second;
				c_two.lat = two.first;
				c_two.lng = two.second;
				all_r.r_length += ComputeDistance(c_one, c_two);
			}
		}
		else {
			all_r.bus = bus; all_r.stops = 0;
		}
		return all_r;
	}
private:
	deque<Busptr> buses_;
	deque<Stop> stops_;
	std::unordered_map<std::string, Stop*> stop_name_to_stop_;
	std::unordered_map<std::string, Bus*> bus_name_to_bus_;



	int countUnique(std::vector<string> vec) {
		std::sort(vec.begin(), vec.end());
		auto last = std::unique(vec.begin(), vec.end());
		return std::distance(vec.begin(), last);
	}
};

class InputReader {
public:
	InputReader(istream& is) : is_(is) {
	}

	int getNumUpdateQueries() {
		int query_count;
		cin /*is_*/ >> num_update_q_;
		return num_update_q_;
	}

	void FillRequests() {
		UpdateReq q;
		for (int i = 0; i < num_update_q_; ++i) {
			string line;
			getline(is_, line);
			auto pos_colon = line.find(":");
			string request_section = line.substr(0, pos_colon);
			string list_section = line.substr(pos_colon + 1);
			auto space_colon = line.find(" ");
			string req_name = request_section.substr(0, space_colon);

			if (req_name == "Bus"s) {
				Bus q;
				auto circle = list_section.find(">");
				if (circle) {
					vector<string> bus_stops = SplitStringBySignCircle(list_section);
					q.type = "c"s;
					q.bus = request_section.substr(space_colon + 1, pos_colon);
					q.stops = bus_stops;
					upd_req_bus_.push_back(q);
				}
				else {
					vector<string> bus_stops = SplitStringBySignPlain(list_section);
					q.type = "p"s;
					q.bus = request_section.substr(space_colon + 1, pos_colon);
					q.stops = bus_stops;
					upd_req_bus_.push_back(q);
				}
			}
			else if (req_name == "Stop"s) {
				Stop q;
				q.stop = request_section.substr(space_colon + 1, pos_colon);
				pair<double, double> coordinates = SplitStringByComma(list_section);
				q.coordinates.lat = coordinates.first;
				q.coordinates.lng = coordinates.second;
				upd_req_stop_.push_back(q);
			}
			else { continue; }
		}
	}

		Void GetUpdStop(TransportCatalogue & tc) {
			for (int i = 0; i < upd_req_stop_.size(); ++i) {
				tc.AddStop(upd_req_stop_[i]);
			}
		}

		Void GetUpdBus(TransportCatalogue & tc) {
			for (int i = 0; i < upd_req_bus_.size(); ++i) {
				tc.AddBus(upd_req_bus_[i]);
			}
		}

private:
	istream& is_;
	deque<Bus> upd_req_bus_;
	deque<Stop> upd_req_stop_;
	int num_update_q_;
	};

int main()
{
	InputReader reader(std::cin);
	int count = reader.getNumUpdateQueries();
	std::cout << count << std::endl;
	for (int i = 0; i < count; i++) {
		UpdateQuery q = reader.getUpdateQuery();
		std::cout << int(q.type) << std::endl;
	}
}
