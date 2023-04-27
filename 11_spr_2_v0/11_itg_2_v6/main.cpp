#include "geo.h"
#include "transport_catalogue.h"
#include "json_reader.h"
#include "iostream"
#include "map_renderer.h"
#include "request_handler.h"

using namespace transport_catalogue;
using namespace std;

// проверка можно ли закинуть струкутру с ответом в json
struct AllBusInfoBusResponse {
	int id = 25;
	std::string bus_name = "BB";
	int quant_stops = 10;
	int quant_uniq_stops = 9;
	double route_length = 10;
	double route_curvature = 1.30;
};



// тут проверочный запуск функции вне класса 

struct OutputRequest {
	int id;
	std::string type;
	std::string name;
};

struct StopDistancesDescription {
	std::string stop_name;
	std::vector<std::pair<std::string, int>> distances;
};

struct Stop {
	std::string stop_name;
	geo::Coordinates coordinates;

};

struct BusDescription {
	std::string bus_name;
	std::vector<std::string> stops;
	std::string type;
};

std::deque<::BusDescription> upd_req_bus_;

std::deque<::Stop> upd_req_stop_;
std::vector<::StopDistancesDescription> distances_;
std::deque<::OutputRequest> out_req_;

std::ostream& operator<<(std::ostream& os, const Render_data& render_data) {
	os << "Render Data: \n"
		<< "  Width: " << render_data.width << "\n"
		<< "  Height: " << render_data.height << "\n"
		<< "  Padding: " << render_data.padding << "\n"
		<< "  Line Width: " << render_data.line_width << "\n"
		<< "  Stop Radius: " << render_data.stop_radius << "\n"
		<< "  Bus Label Font Size: " << render_data.bus_label_font_size << "\n"
		<< "  Bus Label Offset: ";
	for (const auto& offset : render_data.bus_label_offset) {
		os << offset << " ";
	}
	os << "\n  Stop Label Font Size: " << render_data.stop_label_font_size << "\n"
		<< "  Stop Label Offset: ";
	for (const auto& offset : render_data.stop_label_offset) {
		os << offset << " ";
	}
	os << "\n  Underlayer Color: " << render_data.underlayer_color << "\n"
		<< "  Underlayer Width: " << render_data.underlayer_width << "\n"
		<< "  Color Palette: ";
	for (const auto& color : render_data.color_palette) {
		os << color << " ";
	}
	os << "\n";

	return os;
}



void ReadInputJsonRequest() {

	using namespace std::literals;
	auto doc = json::Load(std::cin);

	const auto& json_array = ((doc.GetRoot()).AsMap()).at("base_requests"s);
	const auto& json_array_out = ((doc.GetRoot()).AsMap()).at("stat_requests"s);
	auto& json_array_render = ((doc.GetRoot()).AsMap()).at("render_settings"s);

	for (const auto& file : json_array.AsArray()) {
		const auto& json_obj = file.AsMap();
		if (json_obj.at("type"s) == "Stop"s) {
			::Stop stopjson;
			stopjson.stop_name = json_obj.at("name").AsString();
			stopjson.coordinates.lat = json_obj.at("latitude").AsDouble();
			stopjson.coordinates.lng = json_obj.at("longitude").AsDouble();

			cout << "stopjson.stop_name: " << stopjson.stop_name << " stopjson.coordinates.lat :" << stopjson.coordinates.lat << " stopjson.coordinates.lng :" << stopjson.coordinates.lng << endl;

			::StopDistancesDescription input_stop_dist;
			input_stop_dist.stop_name = json_obj.at("name").AsString();
			auto heighbors = json_obj.at("road_distances");

			for (auto el : heighbors.AsMap()) {
				input_stop_dist.distances.push_back(make_pair(el.first, el.second.AsInt()));
				cout << "el.first : " << el.first << " el.second.AsInt() : " << el.second.AsInt() << endl;
			}
			//upd_req_stop_.push_back(stopjson);
			//distances_.push_back(input_stop_dist);

		}
		else if (json_obj.at("type"s) == "Bus"s) {
			::BusDescription bs;
			auto stop_list = json_obj.at("stops").AsArray();
			for (auto el : stop_list) {

				bs.stops.push_back(el.AsString());

				cout << "el.AsString() : " << el.AsString() << endl;

			}
			bs.bus_name = json_obj.at("name").AsString();
			cout << "bus_name : " << json_obj.at("name").AsString() << endl;

			if (json_obj.at("is_roundtrip").AsBool()) { bs.type = "true"s; }
			else { bs.type = "false"s; }

			cout << "bs.type: "s << bs.type << endl;
			upd_req_bus_.push_back(bs);

		}
	}
	for (auto el : upd_req_bus_) {
		cout << "el.bus_name  " << el.bus_name << " el.type  " << el.type << endl;
		cout << "bus_stops : " << endl;
		for (auto i : el.stops) {
			cout << i << endl;
		}
	}

	for (auto el : upd_req_stop_) {
		cout << "el.stop_name  " << el.stop_name << " el.latitude  " << el.coordinates.lat << " el.lng  " << el.coordinates.lng << endl;
	}
	for (auto el : distances_) {
		cout << "distances : " << endl;
		cout << "el.stop_name  " << el.stop_name << endl;
		cout << "stops : " << endl;
		for (auto i : el.distances) { cout << i.first << "  : " << "dist : " << i.second << endl; }

	}

	for (const auto& file : json_array_out.AsArray()) {
		const auto& json_obj = file.AsMap();
		::OutputRequest outputstopjson;
		outputstopjson.name = json_obj.at("name").AsString();
		outputstopjson.id = json_obj.at("id").AsInt();
		outputstopjson.type = json_obj.at("type").AsString();
		out_req_.push_back(outputstopjson);
	}

	for (auto el : out_req_) {
		cout << "output : " << endl;
		cout << "el.id  " << el.id << endl;
		cout << "el.name : " << el.name <<  " el.type " << el.type<<  endl;

	}
	
	struct Render_data render_data_;
	for (const auto& file : json_array_render.AsArray()) {
		const auto& json_obj = file.AsMap();
		

		render_data_.width = json_obj.at("width").AsDouble();
		render_data_.height = json_obj.at("height").AsDouble();

		render_data_.padding = json_obj.at("padding").AsDouble();
		render_data_.line_width = json_obj.at("line_width").AsDouble();
		render_data_.stop_radius = json_obj.at("stop_radius").AsDouble();
		render_data_.bus_label_font_size = json_obj.at("bus_label_font_size").AsInt();



		for (auto el : json_obj.at("bus_label_offset").AsArray()) { render_data_.bus_label_offset.push_back(el.AsDouble()); }


		render_data_.stop_label_font_size = json_obj.at("stop_label_font_size").AsInt();

		for (auto el : json_obj.at("stop_label_offset").AsArray()) { render_data_.stop_label_offset.push_back(el.AsDouble()); }

		render_data_.underlayer_color.red = static_cast<uint8_t>(json_obj.at("underlayer_color").AsArray()[0].AsInt());
		render_data_.underlayer_color.green = static_cast<uint8_t>(json_obj.at("underlayer_color").AsArray()[1].AsInt());
		render_data_.underlayer_color.blue = static_cast<uint8_t>(json_obj.at("underlayer_color").AsArray()[2].AsInt());
		render_data_.underlayer_color.opacity = static_cast<uint8_t>(json_obj.at("underlayer_color").AsArray()[3].AsDouble());

		render_data_.underlayer_width = json_obj.at("underlayer_width").AsDouble();


		for (auto el : json_obj.at("color_palette").AsArray()) {
			if (el.IsString()) {
				render_data_.color_palette.push_back(el.AsString());
			}
			else {
				if (el.AsArray().size() == 3) {
					Rgb rgb;
					rgb.red = static_cast<uint8_t>(json_obj.at("underlayer_color").AsArray()[0].AsInt());
					rgb.green = static_cast<uint8_t>(json_obj.at("underlayer_color").AsArray()[1].AsInt());
					rgb.blue = static_cast<uint8_t>(json_obj.at("underlayer_color").AsArray()[2].AsInt());
					render_data_.color_palette.push_back(rgb);
				}
				else if (el.AsArray().size() == 4) {
					Rgba rgba;
					rgba.red = static_cast<uint8_t>(json_obj.at("underlayer_color").AsArray()[0].AsInt());
					rgba.green = static_cast<uint8_t>(json_obj.at("underlayer_color").AsArray()[1].AsInt());
					rgba.blue = static_cast<uint8_t>(json_obj.at("underlayer_color").AsArray()[2].AsInt());
					rgba.opacity = static_cast<uint8_t>(json_obj.at("underlayer_color").AsArray()[3].AsDouble());
					render_data_.color_palette.push_back(rgba);
				}



			}

			
		}
		
	}
	std::cout << render_data_ << std::endl;
	
}
struct OutputRequestTest {
	int id = 11;
	std::string type = "Bus";
	std::string name = "FRGR_BUS";
};


int main() {

	
	//DrawPolyline(); // проверить что рисует линию маршрута одного 
	ReadInputJsonRequest(); //просто вызвать функцию по чтению json которая эквив чтению в классе чтобы проверить что все работает

	/*
	transport_catalogue::TransportCatalogue tc;
	transport_catalogue::InputReaderJson reader(std::cin);
	(void)reader.ReadInputJsonRequest();

	reader.UpdStop(tc);
	reader.UpdBus(tc);
	reader.UpdStopDist(tc);

	reader.ManageOutputRequests(tc);
	RequestToTc rtotc;
	Render_data render_data = reader.GetRenderData();

	DrawRoute(rtotc, render_data);   // тут уже отрисовка маршрута 
	*/
	
	
	return 0;


}
