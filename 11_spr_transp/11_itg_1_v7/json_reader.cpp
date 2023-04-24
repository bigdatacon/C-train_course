#include "transport_catalogue.h"
#include "json.h"
#include "geo.h"
#include "json_reader.h"


using namespace json;
using namespace std;


namespace transport_catalogue {


	InputReaderJson::InputReaderJson(istream& is) : is_(is) {
	}

	void InputReaderJson::ReadInputJsonRequest() {

		using namespace std::literals;
		//auto doc = json::Load(std::cin);
		auto doc = json::Load(is_);
		const auto& json_array = ((doc.GetRoot()).AsMap()).at("base_requests"s);

		for (const auto& file : json_array.AsArray()) {
			const auto& json_obj = file.AsMap();
			if (json_obj.at("type"s) == "Stop"s) {
				Stop stopjson;
				stopjson.stop_name = json_obj.at("name").AsString();
				stopjson.coordinates.lat = json_obj.at("latitude").AsDouble();
				stopjson.coordinates.lng = json_obj.at("longitude").AsDouble();

				StopDistancesDescription input_stop_dist;
				input_stop_dist.stop_name = json_obj.at("name").AsString();
				auto heighbors = json_obj.at("road_distances");

				for (auto el : heighbors.AsMap()) {
					input_stop_dist.distances.push_back(make_pair(el.first, el.second.AsInt()));

				}
				upd_req_stop_.push_back(stopjson);
				distances_.push_back(input_stop_dist);
			}
			else if (json_obj.at("type"s) == "Bus"s) {
				BusDescription bs;
				auto stop_list = json_obj.at("stops");
				for (auto el : stop_list.AsArray()) {

					bs.stops.push_back(el.AsString());

				}
				bs.bus_name = json_obj.at("name").AsString();

				if (json_obj.at("is_roundtrip").AsBool()) { bs.type = "true"s; }
				else { bs.type = "false"s; }
				upd_req_bus_.push_back(bs);

			}
		}

	}

	void InputReaderJson::ReadOutputJsonRequest() {

		using namespace std::literals;
		auto doc = json::Load(std::cin);
		const auto& json_array = ((doc.GetRoot()).AsMap()).at("stat_requests"s);

		for (const auto& file : json_array.AsArray()) {
			const auto& json_obj = file.AsMap();
			OutputRequest outputstopjson;
			outputstopjson.name = json_obj.at("name").AsString();
			outputstopjson.id = json_obj.at("id").AsInt();
			outputstopjson.type = json_obj.at("type").AsString();
			out_req_.push_back(outputstopjson);
		}
	}

		void InputReaderJson::UpdStop(TransportCatalogue & tc) {
			for (int i = 0; i < upd_req_stop_.size(); ++i) {
				tc.AddStop(upd_req_stop_[i]);
			}
		}

		void InputReaderJson::UpdStopDist(TransportCatalogue & tc) {
			for (int i = 0; i < distances_.size(); ++i) {
				tc.AddStopDistance(distances_[i]);
			}
		}

		void InputReaderJson::UpdBus(TransportCatalogue & tc) {
			for (int i = 0; i < upd_req_bus_.size(); ++i) {
				tc.AddBus(upd_req_bus_[i]);
			}
		}

		void InputReaderJson::ManageOutputRequests(TransportCatalogue& tc) {
			for (auto el : out_req_) {
				if (el.type == "Bus"s) {
					AllBusInfoBusResponse r = tc.GetAllBusInfo(el.name);
					if (r.quant_stops == 0) {
						cout << "Bus " << el.id << ": "s << "not found" << endl;
					}
					else {
						cout << "Bus " << r.bus_name << ": "s << r.quant_stops << " stops on route, "s << r.quant_uniq_stops << " unique stops, "s << r.route_length << " route length, "s << r.route_curvature << " curvature"s << endl;
					}
				}

				if (el.name == "Stop"s) {
					const Stop* myStop = tc.FindStop(el.name);
					if (myStop == nullptr) {
						cout << "Stop " << el.id << ": not found" << endl;

					}
					else {
						set<string> r = tc.GetStopInfo(el.name);
						if (r.size() == 0) {
							cout << "Stop " << el.id << ": no buses" << endl;
						}
						else {
							cout << "Stop " << el.id << ": "s << "buses ";
							for (const auto& el : r) { cout << el << " "; }
							cout << endl;
						}
					}
				}
			
			
			}
		
		};

	}
