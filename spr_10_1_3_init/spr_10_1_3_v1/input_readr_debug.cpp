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
    int stops;
    int uniq_stops;
    int r_length;
};

class TransportCatalogue {
public:
    void AddBus(const Query q) {
        buses_[q.bus.bus] = q.bus.stops;
    }

    void AddStop(const Query q) {
        stops_[q.stop.stop] = make_pair(q.stop.coordinates.lat, q.stop.coordinates.lng);
    }

    vector<string> FindBus(const string bus) {
        vector<string> res;
        if (buses_.count(bus)) { return buses_[bus]; }
        else { cout << "Bus " << bus << ": not found" << endl; return res; }
    }

    pair<double, double> FindStop(const string stop) {
        pair<double, double> res;
        if (stops_.count(stop)) { return stops_[stop]; }
        else { cout << "Stop " << stop << ": not found" << endl; return res; }
    }

    AllBusInfoBusResponse GetAllBusInfo(const string bus) {
        //6 stops on route, 5 unique stops, 4371.02 route length
        AllBusInfoBusResponse all_r;

        /*int stops;
        int uniq_stops;
        int r_length ;*/
        vector<string> stops_v = FindBus(bus);
        all_r.stops = stops_v.size();
        all_r.uniq_stops = countUnique(stops_v);
        // подсчет расстояния ComputeDistance
        for (int i = 0; i < stops_v.size() - 1; i++) {
            pair<double, double> one = FindStop(stops_v[i]);
            pair<double, double> two = FindStop(stops_v[i + 1]);


            Coordinates c_one;
            Coordinates c_two;

            c_one.lat = one.first;
            c_one.lng = one.second;
            c_two.lat = two.first;
            c_two.lng = two.second;
            all_r.r_length += ComputeDistance(c_one, c_two);
        }
        //Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length
        //cout << "Dus " << bus << ":"s << stops << " stops on route, "s << uniq_stops << " unique stops, "s <<r_length << " route length"s  << endl;
    }


private:
    unordered_map<string, vector<string>> buses_;
    unordered_map<string, pair<double, double>> stops_;

    int countUnique(std::vector<string> vec) {
        std::sort(vec.begin(), vec.end());
        auto last = std::unique(vec.begin(), vec.end());
        return std::distance(vec.begin(), last);
    }

};


int main() {
    deque<pair <string, string>> deq_; // тут перечень запросов на вывод 
    TransportCatalogue tc;


    int query_count;
    Query q;
    cin >> query_count;
    for (int i = 0; i < query_count; ++i) {
        cin >> q;
        switch (q.type) {
        case QueryType::Bus:
            tc.AddBus(q);
            //break;
        case QueryType::Stop:
            tc.AddStop(q);
            //break;
        }
   

        /*int query_count2;
        cin >> query_count2;

        for (int j = 0; j < query_count2; ++j) {
            string line;
            // После заполнения базы читаю и записываю запросы на вывод 
            while (std::getline(std::cin, line)) {
                auto space_colon = line.find_first_not_of(" ");  // : отделяет название запроса
                string type_req = line.substr(0, space_colon);
                string number = line.substr(space_colon);
                deq_.push_back(std::make_pair(type_req, number));

            }
        }*/


    }
    return 0;
};

