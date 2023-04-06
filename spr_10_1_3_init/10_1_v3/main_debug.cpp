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

using namespace std;




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

struct AllBusInfoBusResponse {
	// Наполните полями эту структуру
	string bus;
	int stops;
	int uniq_stops;
	int r_length;
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
struct Stop {
	string stop;
	Coordinates coordinates;
};

struct Bus {
	string bus;
	vector<string> stops;
	string type;
};
struct Busptr {
	string bus;
	deque<string*> stops;
	string type;
};

class TransportCatalogue {
public:
	void AddBus(Bus b) {
		Busptr bptr;
		deque <string*> stops_ptr;
		// тут нужно в buses добавить вместо string названий остановок - ссылки на остановки 
		for (auto stop : b.stops) {
			auto it = stop_name_to_stop_.find(stop);  
			if (it != stop_name_to_stop_.end()) {
				Stop stopPtr = *(it->second);
				string stopNamePtr = stopPtr.stop;
				stops_ptr.push_back(&stopNamePtr);
			}
		}
		bptr.bus = b.bus;
		bptr.type = b.type;  // sv
		bptr.stops = stops_ptr;
		buses_.push_back(bptr); //move
		Busptr* bptr_bus = &bptr;
		bus_name_to_bus_.emplace(b.bus, bptr_bus);

	}

	void AddStop(Stop s) {
		Stop* ptr_stop = &s;
		stop_name_to_stop_.emplace(s.stop, ptr_stop);
		stops_.push_back(s); //move
	}

	Busptr FindBus(string bus) {
		Busptr res;
		if (bus_name_to_bus_.count(bus)) { return *bus_name_to_bus_[bus]; }
		else { cout << "Bus " << bus << ": not found" << endl; return res; }
	}

	Stop FindStop(string stop) {
		Stop res;
		if (stop_name_to_stop_.count(stop)) { return *stop_name_to_stop_[stop]; }
		else { cout << "Stop " << stop << ": not found" << endl; return res; }
	}

	AllBusInfoBusResponse GetAllBusInfo( string bus) {
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
				Stop one = FindStop(*stops_v[i]);
				Stop two = FindStop(*stops_v[i + 1]); 
				++lap;
				all_r.r_length += ComputeDistance(one.coordinates, two.coordinates);
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
	std::unordered_map<std::string, Busptr*> bus_name_to_bus_;
};

class InputReader {
public:
	InputReader(istream& is) : is_(is) {
	}

	int GetNumUpdateQueries() {
		string line;
		getline(is_, line);
		num_update_q_ = std::stoi(line);
		return num_update_q_;
	}

	void FillRequests() {
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

	void GetUpdStop(TransportCatalogue& tc) {
		for (int i = 0; i < upd_req_stop_.size(); ++i) {
			cout << "UPDT : " << upd_req_stop_[i].stop << endl;
			tc.AddStop(upd_req_stop_[i]);
		}
	}

	void GetUpdBus(TransportCatalogue& tc) {
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
	TransportCatalogue tc;
    InputReader reader(std::cin);
    int count = reader.GetNumUpdateQueries();
    std::cout << "COUNT : " << count << std::endl;
    for (int i = 0; i < count; i++) {
		reader.FillRequests();
    }
	reader.GetUpdStop(tc);

    return 0;
}
