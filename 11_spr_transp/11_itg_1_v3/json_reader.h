#pragma once
#include <cassert>
#include <chrono>
#include <sstream>
#include <string_view>
#include <iostream>
#include <fstream>
#include <string>

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


std::vector<std::string> GetStringRequests(std::string full_line) {
    std::vector<std::string> itg_vec;
    //1 ќбреза€ от строки сначала блок "base_requests" - чтобы убрать "base_requests" и оставить только что в нем
    std::size_t base_requests_pos = full_line.find("\"base_requests\":");
    std::size_t brace_pos = full_line.find('{', base_requests_pos);
    std::string stripped_line = full_line.substr(brace_pos);

    // 3 обрезаю до "stat_requests"
    std::string stripped_line_no_statr = stripped_line.substr(0, stripped_line.find("\"stat_requests\""));


    // 4 удал€ю пробелы с конца и ],
    size_t last_non_space = stripped_line_no_statr.find_last_not_of(" ");
    if (last_non_space != std::string::npos) {
        stripped_line_no_statr = stripped_line_no_statr.substr(0, last_non_space + 1);
    }

    // ”даление знаков '], с конца'
    size_t last_bracket = stripped_line_no_statr.find_last_of("]");
    if (last_bracket != std::string::npos) {
        stripped_line_no_statr = stripped_line_no_statr.substr(0, last_bracket);
    }

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    //II отдельно делаю строку в формате map дл€ stat_requests

    itg_vec.push_back(stripped_line_no_statr);

    //5 оставл€ю только то что в stat_requests
    std::size_t pos = stripped_line.find("\"stat_requests\":");
    if (pos != std::string::npos) {
        stripped_line = stripped_line.substr(pos + 16);
    }

    //6 ”даление пробелов и скобок
    size_t last_non_space2 = stripped_line.find_last_not_of(" ");
    if (last_non_space2 != std::string::npos) {
        stripped_line = stripped_line.substr(0, last_non_space2 - 1);
    }

    size_t start = stripped_line.find_first_not_of("[ ");
    size_t end = stripped_line.find_last_not_of(" ]");
    std::string stat_reader = stripped_line.substr(start, end);
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    //std::cout << "ITG : " << stat_reader << '\n';
    itg_vec.push_back(stat_reader);
    return itg_vec;

}

vector<string> ParsString(string stat_r) {
    // разбиаю stat_r на подстроки по знака },
    vector<string> result;
    std::size_t pos = 0;
    std::size_t endpos;

    while ((endpos = stat_r.find("},", pos)) != std::string::npos) // ищем все объекты JSON в строке
    {
        std::string jsonStr = stat_r.substr(pos, endpos - pos + 1); // объект JSON
        //std::cout << jsonStr << std::endl;
        result.push_back(jsonStr);
        pos = endpos + 2; // продолжаем поиск со следующего символа после разделител€
    }

    // обрабатываем последний объект JSON
    std::string jsonStr = stat_r.substr(pos);
    //std::cout << jsonStr << std::endl;
    result.push_back(jsonStr);
    return result;

}

inline void ReadInputJsonRequest() {
    std::ifstream inFile("input.json.txt"); // открытие файла дл€ чтени€
    if (!inFile) { // проверка успешности открыти€ файла
        std::cerr << "ќшибка при открытии файла " << std::endl;
        return;
    }

    std::string full_line;
    std::string line;
    while (std::getline(inFile, line)) {
    //while (std::getline(std::cin, line)) {
        full_line += line;
    }
    // –азабираю запрос на 2 строки с подзапросами дл€ base и stat
    string stat_r = GetStringRequests(full_line)[1];
    string base_r = GetStringRequests(full_line)[0];

    vector<string> stat_result = ParsString(stat_r);
    vector<string> base_result = ParsString(base_r);

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


    for (string s : stat_result) {
        cout << "stat" << endl;
        json::Document stat_req = LoadJSON(s);
        Print(stat_req, cout);
        cout << endl;
    }

    for (string s : base_result) {
        cout << "base" << endl;
        json::Document base_req = LoadJSON(s);
        Print(base_req, cout);
        cout << endl;
    }

    std::deque<OutputRequestStop> out_req_stop;
    std::deque<OutputRequestBus> out_req_bus;

    std::deque<BusDescriptionJson> upd_req_bus;
    std::deque<StopJson> upd_req_stop;
    std::vector<StopDistancesDescriptionJson> distances;


    OutputRequestBus out_bus;

    for (string s : stat_result) {

        json::Document stat_req = LoadJSON(s);
        if (stat_req.GetRoot().AsMap().at("type").AsString() == "Stop") {
            OutputRequestStop out_stop;
            out_stop.id = stat_req.GetRoot().AsMap().at("id").AsInt();
            out_stop.stop_name = stat_req.GetRoot().AsMap().at("name").AsString();
            out_req_stop.push_back(out_stop);
        }
        else {
            OutputRequestBus out_bus;
            out_bus.id = stat_req.GetRoot().AsMap().at("id").AsInt();
            out_bus.bus_name = stat_req.GetRoot().AsMap().at("name").AsString();
            out_req_bus.push_back(out_bus);

        }
    }

    for (string s : base_result) {
        json::Document base_req = LoadJSON(s);
        if (base_req.GetRoot().AsMap().at("type").AsString() == "Stop") {
            StopJson input_stop;

            input_stop.stop_name = base_req.GetRoot().AsMap().at("name").AsString();
            input_stop.coordinates.lat = base_req.GetRoot().AsMap().at("latitude").AsDouble();
            input_stop.coordinates.lng = base_req.GetRoot().AsMap().at("longitude").AsDouble();

            auto dist = base_req.GetRoot().AsMap().at("road_distances").AsString();
            cout << "Here dist : " << dist << endl;

            if (base_req.GetRoot().AsMap().at("road_distances").AsMap().size() != 0) {
                StopDistancesDescriptionJson input_stop_dist;
                input_stop_dist.stop_name = base_req.GetRoot().AsMap().at("name").AsString();

                auto heighbors = base_req.GetRoot().AsMap().at("road_distances").AsMap();
                /*for (auto const& [name, distance] : heighbors) {
                    input_stop_dist.distances[name] = distance;
                }*/


                distances.push_back(input_stop_dist);
            }
            upd_req_stop.push_back(input_stop);


        }
        else {
            BusDescriptionJson input_bus;
            input_bus.bus_name = base_req.GetRoot().AsMap().at("name").AsString();
            input_bus.type = base_req.GetRoot().AsMap().at("is_roundtrip").AsString()[0];
            auto stops_in_bus = base_req.GetRoot().AsMap().at("stops");
            /*for (auto el : stops_in_bus.AsArray()) {
                input_bus.stops.push_back(el);
            }*/
            upd_req_bus.push_back(input_bus);

        }
    }

}

inline void ReadInputJsonRequest2() {
    
    /*auto input_json = Load(std::cin).GetRoot().AsMap();
    auto base_input = input_json.at("base_request").AsArray();
    for (auto el : base_input) {
        auto first = el.AsMap();
    
    }*/

    using namespace std::literals;
    auto doc = json::Load(std::cin);
    const auto& json_array = ((doc.GetRoot()).AsMap()).at("base_requests"s);

    for (const auto& file : json_array.AsArray()) {
        const auto& json_obj = file.AsMap();
        if (json_obj.at("type"s) == "Stop"s) {
            std::cout << "type"s  <<  std::endl;
            continue;
        }
        else if (json_obj.at("type"s) == "Bus"s) {
            std::cout << std::endl;
            continue;
        }
    }

    
    
 }




