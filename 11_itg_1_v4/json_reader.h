#pragma once
#include <cassert>
#include <chrono>
#include <sstream>
#include <string_view>
#include <iostream>
#include <fstream>
#include <string>
#include <deque>

#include "json.h"
#include "geo.h"


using namespace json;
using namespace std::literals;
using namespace std;

using namespace std;

inline json::Document LoadJSON(const std::string& s) {
    std::istringstream strm(s);
    return json::Load(strm);
}

inline std::string Print(const Node& node) {
    std::ostringstream out;
    Print(Document{ node }, out);
    return out.str();
}



struct StopDistancesDescriptionJson {
    std::string stop_name;
    std::vector<std::pair<std::string, int>> distances;
};

struct StopJson {
    std::string stop_name;
    geo::Coordinates coordinates;
};

struct BusDescriptionJson {
    std::string bus_name;
    std::vector<std::string> stops;
    std::string type;
};

struct OutputRequestStop {
    int id;
    std::string stop_name;
};

struct OutputRequestBus {
    int id;
    std::string bus_name;
};

std::deque<OutputRequestStop> out_req_stop;
deque<OutputRequestBus> out_req_bus;

std::deque<BusDescriptionJson> upd_req_bus;
std::deque<StopJson> upd_req_stop;
std::vector<StopDistancesDescriptionJson> distances;

inline void ReadInputJsonRequest2() {
    



    OutputRequestBus out_bus;

    using namespace std::literals;
    auto doc = json::Load(std::cin);
    const auto& json_array = ((doc.GetRoot()).AsMap()).at("base_requests"s);

    for (const auto& file : json_array.AsArray()) {
        const auto& json_obj = file.AsMap();
        if (json_obj.at("type"s) == "Stop"s) {
            StopJson stopjson;
            stopjson.stop_name = json_obj.at("name").AsString();
            stopjson.coordinates.lat = json_obj.at("latitude").AsDouble();
            stopjson.coordinates.lng = json_obj.at("longitude").AsDouble();


            StopDistancesDescriptionJson input_stop_dist;
            input_stop_dist.stop_name = json_obj.at("name").AsString();
            auto heighbors = json_obj.at("road_distances");

            for (auto el : heighbors.AsMap()) {
                input_stop_dist.distances.push_back(make_pair(el.first, el.second.AsInt()));
            
            }
            //for (auto el : input_stop_dist.distances) { cout << el.first << " "s << el.second << endl; }

            upd_req_stop.push_back(stopjson);
            distances.push_back(input_stop_dist);

            
        } 
        else if (json_obj.at("type"s) == "Bus"s) {
            //std::cout << std::endl;
            //continue;
            auto stop_list = json_obj.at("stops");
            BusDescriptionJson bs;
            for (auto el : stop_list.AsArray()) {
                
                bs.stops.push_back(el.AsString());
                    
            }
            bs.bus_name = json_obj.at("name").AsString();
            //if (json_obj.at("is_roundtrip").AsString()[0] == 'f') { bs.type = "false"s; }
            //else { bs.type = "true"s; }

            if (json_obj.at("is_roundtrip").AsBool()) { bs.type = "true"s; }
            else { bs.type = "false"s; }

            //bs.type = json_obj.at("is_roundtrip").AsString();

            cout <<"bs.bus_name :  " << bs.bus_name << endl;
            cout << "type : " << bs.type << endl;

            for (auto el : bs.stops) {

                cout << "stop : " << el << endl;

            }
            /*for (auto el : bs.stops) {
                cout << "bs.stops: " << endl;
                cout << "stop : " << el << endl;*/

            //cout << bs.bus_name << "type : " << bs.type << endl;

        }
    }

    
    
    
 }




