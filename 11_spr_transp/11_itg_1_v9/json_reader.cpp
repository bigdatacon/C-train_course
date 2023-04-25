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

		const auto& json_array_out = ((doc.GetRoot()).AsMap()).at("stat_requests"s);

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
				auto stop_list = json_obj.at("stops").AsArray();
				for (auto el : stop_list) {

					bs.stops.push_back(el.AsString());

				}
				bs.bus_name = json_obj.at("name").AsString();

				if (json_obj.at("is_roundtrip").AsBool()) { bs.type = "true"s; }
				else { bs.type = "false"s; }
				upd_req_bus_.push_back(bs);

			}
		}

		for (const auto& file : json_array_out.AsArray()) {
			const auto& json_obj = file.AsMap();
			OutputRequest outputstopjson;
			outputstopjson.name = json_obj.at("name").AsString();
			outputstopjson.id = json_obj.at("id").AsInt();
			outputstopjson.type = json_obj.at("type").AsString();
			out_req_.push_back(outputstopjson);
		}


	}


	void InputReaderJson::UpdStop(TransportCatalogue& tc) {
		for (int i = 0; i < upd_req_stop_.size(); ++i) {
			tc.AddStop(upd_req_stop_[i]);
		}
	}

	void InputReaderJson::UpdStopDist(TransportCatalogue& tc) {
		for (int i = 0; i < distances_.size(); ++i) {
			tc.AddStopDistance(distances_[i]);
		}
	}

	void InputReaderJson::UpdBus(TransportCatalogue& tc) {
		for (int i = 0; i < upd_req_bus_.size(); ++i) {
			tc.AddBus(upd_req_bus_[i]);
		}
	}

	void InputReaderJson::ManageOutputRequests(TransportCatalogue& tc) {
		std::ostream& out = std::cout;
		json::Array queries;
		for (auto el : out_req_) {
			if (el.type == "Bus"s) {
				AllBusInfoBusResponse r = tc.GetAllBusInfo(el.name);
				if (r.quant_stops == 0) {
					json::Node answer_empty_bus = json::Dict{
							{"request_id", el.id},
							{"error_message" ,  "not found"}
					};
					queries.emplace_back(answer_empty_bus);

				}
				else {
					json::Node non_empty_bus = json::Dict{
							{"curvature", r.route_curvature},
							{"request_id" , el.id},
							{"route_length" , r.route_length},
							{"stop_count" , r.quant_stops},
							{"unique_stop_count" , r.quant_uniq_stops}
					};
					queries.emplace_back(non_empty_bus);
				}
			}

			if (el.type == "Stop"s) {
				const Stop* myStop = tc.FindStop(el.name);
				if (myStop == nullptr) {
					json::Node answer_empty_stop = json::Dict{
							{"request_id", el.id},
							{"error_message" ,  "not found"}
					};
					queries.emplace_back(answer_empty_stop);

				}
				else {
					set<string> r = tc.GetStopInfo(el.name);
					json::Array routes;
					std::copy(r.begin(), r.end(), std::back_inserter(routes));
					json::Node answer_stop = json::Dict{
						{"buses", routes},
						{"request_id" ,  el.id}

					};
					queries.emplace_back(answer_stop);

					
				}
			}


		}
		json::Print(json::Document{ queries }, out);

	}

}
