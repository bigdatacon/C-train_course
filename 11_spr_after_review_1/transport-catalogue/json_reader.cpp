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
		auto doc = json::Load(is_);
		const auto& json_array = ((doc.GetRoot()).AsMap()).at("base_requests"s);

		const auto& json_array_out = ((doc.GetRoot()).AsMap()).at("stat_requests"s);
		auto& json_array_render = ((doc.GetRoot()).AsMap()).at("render_settings"s);

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

				auto trip = json_obj.at("is_roundtrip").AsBool();
				if (trip) {
					bs.type = "true"s;
				}
				else { bs.type = "false"s; }
				upd_req_bus_.push_back(bs);

			}
		}

		for (const auto& file : json_array_out.AsArray()) {
			const auto& json_obj = file.AsMap();
			OutputRequest outputstopjson;
			if (json_obj.at("type").AsString() == "Map"s) {
				outputstopjson.id = json_obj.at("id").AsInt();
				outputstopjson.type = json_obj.at("type").AsString();
				out_req_.push_back(outputstopjson);
			
			}
			else {
				outputstopjson.name = json_obj.at("name").AsString();
				outputstopjson.id = json_obj.at("id").AsInt();
				outputstopjson.type = json_obj.at("type").AsString();
				out_req_.push_back(outputstopjson);
			}
		}

		for (const auto& json_obj : json_array_render.AsMap()) {
	
			if (json_obj.first == "width") {
				render_data_.width = json_obj.second.AsDouble();
			}
			else if (json_obj.first == "height") {
				render_data_.height = json_obj.second.AsDouble();
			}
			else if (json_obj.first == "padding") {
				render_data_.padding = json_obj.second.AsDouble();
			}
			else if (json_obj.first == "line_width") {
				render_data_.line_width = json_obj.second.AsDouble();
				
			}
			else if (json_obj.first == "stop_radius") {
				render_data_.stop_radius = json_obj.second.AsDouble();
			}
			else if (json_obj.first == "bus_label_font_size") {
				render_data_.bus_label_font_size = json_obj.second.AsDouble();
			}
			else if (json_obj.first == "bus_label_offset") {
				for (auto el : json_obj.second.AsArray()) { render_data_.bus_label_offset.push_back(el.AsDouble()); }
			}
			else if (json_obj.first == "stop_label_offset") {
				for (auto el : json_obj.second.AsArray()) { render_data_.stop_label_offset.push_back(el.AsDouble()); }
			}
			else if (json_obj.first == "stop_label_font_size") {
				render_data_.stop_label_font_size = json_obj.second.AsInt();
			}
			else if (json_obj.first == "underlayer_color") {
				auto el = json_obj.second;
					if (el.IsString()) {
						render_data_.underlayer_color.push_back(el.AsString());
					}
					else {
						if (el.AsArray().size() == 3) {
							Rgb rgb;
							rgb.red = static_cast<uint8_t>(el.AsArray()[0].AsInt());
							rgb.green = static_cast<uint8_t>(el.AsArray()[1].AsInt());
							rgb.blue = static_cast<uint8_t>(el.AsArray()[2].AsInt());
							render_data_.underlayer_color.push_back(rgb);
						}
						else if (el.AsArray().size() == 4) {
							Rgba rgba;
							rgba.red = static_cast<uint8_t>(el.AsArray()[0].AsInt());
							rgba.green = static_cast<uint8_t>(el.AsArray()[1].AsInt());
							rgba.blue = static_cast<uint8_t>(el.AsArray()[2].AsInt());
							rgba.opacity = el.AsArray()[3].AsDouble();
							render_data_.underlayer_color.push_back(rgba);
						}

					}
				}
	
			else if (json_obj.first == "underlayer_width") {
				render_data_.underlayer_width = json_obj.second.AsDouble();
			}
			else if (json_obj.first == "color_palette") {
				for (auto el : json_obj.second.AsArray()) {
					if (el.IsString()) {
						render_data_.color_palette.push_back(el.AsString());
					}
					else {
						if (el.AsArray().size() == 3) {
							Rgb rgb;
							rgb.red = static_cast<uint8_t>(el.AsArray()[0].AsInt());
							rgb.green = static_cast<uint8_t>(el.AsArray()[1].AsInt());
							rgb.blue = static_cast<uint8_t>(el.AsArray()[2].AsInt());
							render_data_.color_palette.push_back(rgb);
						}
						else if (el.AsArray().size() == 4) {
							Rgba rgba;
							rgba.red = static_cast<uint8_t>(el.AsArray()[0].AsInt());
							rgba.green = static_cast<uint8_t>(el.AsArray()[1].AsInt());
							rgba.blue = static_cast<uint8_t>(el.AsArray()[2].AsInt());
							rgba.opacity = el.AsArray()[3].AsDouble();
							render_data_.color_palette.push_back(rgba);
						}



					}
				}
			}

		}


	}


	void InputReaderJson::UpdStop(TransportCatalogue& tc) {
		for (int i = 0; i < static_cast<int>(upd_req_stop_.size()); ++i) {
			tc.AddStop(upd_req_stop_[i]);
		}
	}

	void InputReaderJson::UpdStopDist(TransportCatalogue& tc) {
		for (int i = 0; i < static_cast<int>(distances_.size()); ++i) {
			tc.AddStopDistance(distances_[i]);
		}
	}

	void InputReaderJson::UpdBus(TransportCatalogue& tc) {
		for (int i = 0; i < static_cast<int>(upd_req_bus_.size()); ++i) {
			tc.AddBus(upd_req_bus_[i]);
		}
	}

	void InputReaderJson::ManageOutputRequests(TransportCatalogue& tc,  MapRenderer& mr) {
		std::ostream& out = std::cout;
		json::Array queries;
		for (auto el : out_req_) {
			if (el.type == "Bus"s) {

				const Bus* bus_resp = tc.FindBus(el.name);
				if (bus_resp == nullptr) {
					json::Node answer_empty_bus = json::Dict{

							{"error_message"s , "not found"s},
							{"request_id", el.id}
					};
					queries.emplace_back(answer_empty_bus);

				}

				else {
					AllBusInfoBusResponse r = tc.GetAllBusInfo(el.name);
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

							{"error_message"s , "not found"s},
							{"request_id", el.id}
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
			if (el.type == "Map"s) {
                string map_str = mr.DrawRouteGetDoc(tc);
				//json::Node map_json = mr.DrawRouteGetNode(tc);

				json::Node answer_empty_map = json::Dict{

						{"map"s , map_str},
						//{"map"s , map_json.AsString()},
						{"request_id", el.id}
				};
				queries.emplace_back(answer_empty_map);
                
				


			}
			

		}
        json::Print(json::Document{ queries }, out);
	}

	Render_data InputReaderJson::GetRenderData() {
		return render_data_;
	}

}
