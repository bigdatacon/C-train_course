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

using namespace std;

/*
10
Stop Tolstopaltsevo: 55.611087, 37.208290
Stop Marushkino: 55.595884, 37.209755
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324
Stop Biryulyovo Zapadnoye: 55.574371, 37.651700
Stop Biryusinka: 55.581065, 37.648390
Stop Universam: 55.587655, 37.645687
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164
3
Bus 256
Bus 750
Bus 751
*/

enum class QueryType {
    Stop,
    Bus
};
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
};

struct Query {
    QueryType type;
    Bus bus;
    Stop stop;
};

vector<string> SplitStringBySign(string str) {

    vector<string> tokens;
    stringstream ss(str);
    string token;

    while (getline(ss, token, '>')) {
        tokens.push_back(token);
    }
    return tokens;
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

istream& operator>>(istream& is, Query& q) {
    string line;
    getline(is, line);
    auto pos_colon = line.find(":");  // : отделяет название автобуса или остановки 
     // : отделяет название запроса 

    string request_section = line.substr(0, pos_colon);
    string list_section = line.substr(pos_colon + 1);
    auto space_colon = line.find(" ");
    string req_name = request_section.substr(0, space_colon);

    if (req_name == "Bus"s) {
        vector<string> bus_stops = SplitStringBySign(list_section);
        q.type = QueryType::Bus  /*"Bus"s*/;
        q.bus.bus = request_section.substr(space_colon+1, pos_colon);
        q.bus.stops = bus_stops;
    }
    else if (req_name == "Stop"s) {
        q.type = QueryType::Stop;
        //q.type = /*QueryType::Bus*/  "Stop"s;
        q.stop.stop = request_section.substr(space_colon+1, pos_colon);
        pair<double, double> coordinates = SplitStringByComma(list_section);
        q.stop.coordinates.lat = coordinates.first;
        q.stop.coordinates.lng = coordinates.second;

        //deq_.push_back(q);
    }
    else if (line == ""s) {
        return is >> q;
    }
    return is;

}


class Input_reader {
public:
    istream& operator>>(istream& is, Query& q, TransportCatalogue tc) {
    int query_count;
    Query q;
    cin >> query_count;
    for (int i = 0; i < query_count; ++i) {
        cin >> q;
        switch (q.type) {
            case QueryType::Bus:
                tc.AddBus(q);
                break;
            case QueryType::Stop:
                tc.AddStop(q);
                break;
         // заполняю класс сначала всеми данными 
    }
        
    int query_count2;
    cin >> query_count2;

    for (int i = 0; i < query_count2; ++i) {
    string line;
    // После заполнения базы читаю и записываю запросы на вывод 
    while (std::getline(std::cin, line)) {
        auto space_colon =  line.find_first_not_of(" ");  // : отделяет название запроса
        string type_req   = line.substr(0, space_colon);
        string number   = line.substr(space_colon);
        deq_.push_back(std::make_pair(type_req, number));
        
    }     
    }

private:
    deque<pair <string, string>> deq_; // тут перечень запросов на вывод 
};
