#include "geo.h"
#include "transport_catalogue.h"
#include "json_reader.h"
#include "iostream"

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

struct OutputRequestStop {
	int id;
	std::string stop_name;
};

struct OutputRequestBus {
	int id;
	std::string bus_name;
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


void ReadInputJsonRequest() {

	using namespace std::literals;
	auto doc = json::Load(std::cin);

	const auto& json_array = ((doc.GetRoot()).AsMap()).at("base_requests"s);

	for (const auto& file : json_array.AsArray()) {
		const auto& json_obj = file.AsMap();
		if (json_obj.at("type"s) == "Stop"s) {
			Stop stopjson;
			stopjson.stop_name = json_obj.at("name").AsString();
			stopjson.coordinates.lat = json_obj.at("latitude").AsDouble();
			stopjson.coordinates.lng = json_obj.at("longitude").AsDouble();

			cout << "stopjson.stop_name: " << stopjson.stop_name << " stopjson.coordinates.lat :" << stopjson.coordinates.lat << " stopjson.coordinates.lng :" << stopjson.coordinates.lng << endl;

			StopDistancesDescription input_stop_dist;
			input_stop_dist.stop_name = json_obj.at("name").AsString();
			auto heighbors = json_obj.at("road_distances");

			for (auto el : heighbors.AsMap()) {
				input_stop_dist.distances.push_back(make_pair(el.first, el.second.AsInt()));
				cout << "el.first : " << el.first << " el.second.AsInt() : " << el.second.AsInt() << endl;
			}

		}
		else if (json_obj.at("type"s) == "Bus"s) {
			BusDescription bs;
			auto stop_list = json_obj.at("stops");
			for (auto el : stop_list.AsArray()) {

				bs.stops.push_back(el.AsString());

				cout << "el.AsString() : " << el.AsString() << endl;

			}
			bs.bus_name = json_obj.at("name").AsString();
			cout << "bus_name : " << json_obj.at("name").AsString()  << endl;

			if (json_obj.at("is_roundtrip").AsBool()) { bs.type = "true"s; }
			else { bs.type = "false"s; }

			cout << "bs.type: "s  << bs.type << endl;

		}
	}

}
struct OutputRequestTest {
	int id = 11;
	std::string type = "Bus";
	std::string name = "FRGR_BUS";
};

int main()
{
	/*std::ostream& out = std::cout;
	AllBusInfoBusResponse tr;
	OutputRequestTest req;
	json::Array queries;
	cout << "tr.bus_name = " << tr.bus_name << endl;

	set<string> r{ "value1", "value2", "value3" };
	json::Array routes; 
	std::copy(r.begin(), r.end(), std::back_inserter(routes));


	json::Node answer_stop = json::Dict{
	{"buses", routes},
	{"request_id" ,  tr.id}

	};
	queries.emplace_back(answer_stop);

	json::Node res = json::Dict{
		{"curvature", tr.route_curvature},
		{"request_id" , tr.id},
		{"route_length" , tr.route_length},
		{"stop_count" , tr.quant_stops},
		{"unique_stop_count" , tr.quant_uniq_stops}
	};
	queries.emplace_back(res);
	cout << "PRINT JSON :" << endl;
	json::Print(json::Document{ queries }, out);*/

	//ReadInputJsonRequest();


	transport_catalogue::TransportCatalogue tc;
	transport_catalogue::InputReaderJson reader(std::cin);
	(void)reader.ReadInputJsonRequest();

	reader.UpdStop(tc);
	reader.UpdBus(tc);
	reader.UpdStopDist(tc);

	reader.ManageOutputRequests(tc);



	return 0;
}

