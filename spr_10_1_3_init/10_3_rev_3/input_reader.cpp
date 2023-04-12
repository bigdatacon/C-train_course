

#include "input_reader.h"
#include "transport_catalogue.h"

using namespace std;


namespace transport_catalogue {

	vector<std::pair<string, int>> SplitStringByForDist(const string& distance_section) {
		vector<std::pair<string, int>> res;
		vector<string> str_set;
		std::stringstream ss(distance_section);
		std::string substring;
		while (std::getline(ss, substring, ',')) {
			str_set.push_back(substring);
		}
		for (string str : str_set) {
			auto first_space_colon = str.find_first_not_of(" ");
			auto last_space_colon = str.find_last_not_of(" ");
			string instance_distance = "m to ";
			auto is_distance = str.find(instance_distance);
			str = str.substr(first_space_colon, last_space_colon + 1);
			string dist_s = str.substr(0, is_distance);
			int dist = std::stoi(str.substr(0, is_distance));
			size_t pos_sec_space = str.find(" ", str.find(" ") + 1);
			string stop_name = str.substr(pos_sec_space + 1);//+1
			res.push_back(pair(stop_name, dist));
		}
		return res;
	}


	vector<string> SplitStringBySignPlain(const string& str) {
		vector<string> tokens;
		stringstream ss(str);
		string token;
		while (getline(ss, token, '-'))
		{
			auto first_space_colon = token.find_first_not_of(" ");  // перенесен тримин пробелов из каталога 
			auto last_space_colon = token.find_last_not_of(" ");
			token = token.substr(first_space_colon, last_space_colon);
			tokens.push_back(token);
		}
		return tokens;
	}

	vector<string> SplitStringBySignCircle(const string& str) {
		vector<string> tokens;
		stringstream ss(str);
		string token;
		while (getline(ss, token, '>'))
		{
			auto first_space_colon = token.find_first_not_of(" ");
			auto last_space_colon = token.find_last_not_of(" ");
			token = token.substr(first_space_colon, last_space_colon);
			tokens.push_back(token);
		}
		return tokens;
	}

	pair<double, double> SplitStringByComma(const string& str) {
		pair<double, double> coordinates;
		stringstream ss(str);
		string token;

		getline(ss, token, ',');
		coordinates.first = stod(token);

		getline(ss, token, ',');
		coordinates.second = stod(token);

		return coordinates;
	}


	InputReader::InputReader(istream& is) : is_(is) {
	}

	int InputReader::GetNumUpdateQueries() {
		string line;
		getline(is_, line);
		num_update_q_ = std::stoi(line);
		return num_update_q_;
	}

	void InputReader::FillRequests() {
		for (int i = 0; i < num_update_q_; ++i) {
			string line;
			getline(is_, line);
			auto pos_colon = line.find(":");
			string request_section = line.substr(0, pos_colon);
			string list_section = line.substr(pos_colon + 1);
			auto space_colon = request_section.find(" "); // ïîìåíÿòü íà request_section
			string req_name = request_section.substr(0, space_colon);
			string instance_distance = "m to ";
			auto is_distance = list_section.find(instance_distance);

			if (req_name == "Bus"s) {
				BusDescription q;
				auto circle = list_section.find(">");
				if (circle != std::string::npos) {
					vector<string> bus_stops = SplitStringBySignCircle(list_section);
					q.type = "c"s;
					q.bus_name = request_section.substr(space_colon + 1, pos_colon);
					q.stops = bus_stops;
					upd_req_bus_.push_back(q);
				}
				else {
					vector<string> bus_stops = SplitStringBySignPlain(list_section);
					q.type = "p"s;
					q.bus_name = request_section.substr(space_colon + 1, pos_colon);
					q.stops = bus_stops;
					upd_req_bus_.push_back(q);
				}
			}
			else if (req_name == "Stop"s && is_distance == std::string::npos) {
				Stop q;
				q.stop_name = request_section.substr(space_colon + 1, pos_colon);
				pair<double, double> coordinates = SplitStringByComma(list_section);
				q.coordinates.lat = coordinates.first;
				q.coordinates.lng = coordinates.second;
				upd_req_stop_.push_back(q);
			}

			else if (req_name == "Stop"s && is_distance != std::string::npos) {
				Stop q;
				q.stop_name = request_section.substr(space_colon + 1, pos_colon);

				size_t pos_sec_comma = list_section.find(",", list_section.find(",") + 1);
				if (pos_sec_comma != std::string::npos) {
					vector<std::pair<string, int>> stop_dist_updt;
					string cordinate_section = list_section.substr(1, pos_sec_comma);
					string distance_section = list_section.substr(pos_sec_comma + 2);

					pair<double, double> coordinates = SplitStringByComma(cordinate_section);
					stop_dist_updt = SplitStringByForDist(distance_section);
					q.coordinates.lat = coordinates.first;
					q.coordinates.lng = coordinates.second;
					q.stop_dist = stop_dist_updt;
				}
				upd_req_stop_.push_back(q);
			}
			else { continue; }
		}
	}

	void InputReader::UpdStop(TransportCatalogue& tc) {
		for (int i = 0; i < upd_req_stop_.size(); ++i) {
			tc.AddStop(upd_req_stop_[i]);
		}
	}

	void InputReader::UpdStopDist(TransportCatalogue& tc) {
		for (int i = 0; i < upd_req_stop_.size(); ++i) {
			tc.AddStopDistance(upd_req_stop_[i]);
		}
	}

	void InputReader::UpdBus(TransportCatalogue& tc) {
		for (int i = 0; i < upd_req_bus_.size(); ++i) {
			tc.AddBus(upd_req_bus_[i]);
		}
	}
}