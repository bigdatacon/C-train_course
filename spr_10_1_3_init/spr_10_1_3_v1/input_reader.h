// напишите решение с нуля
// код сохраните в свой git-репозиторий
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
#include <math.h>
#include <regex>
using namespace std;

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

auto SplitStringBySign(string str, char symbol) {
    if (symbol == ','){
        pair<double, double> coordinates;
        stringstream ss(str);
        string token;
    
        getline(ss, token, ',');
        coordinates.first = stod(token);
    
        getline(ss, token, ',');
        coordinates.second = stod(token);
        
        return coordinates;
    }
    else if (symbol == '>'){
        vector<string> tokens;
        stringstream ss(str);
        string token;

        while (getline(ss, token, '>')) {
            tokens.push_back(token);
        }
    }
    else {return false;}
}

istream& operator>>(istream& is, Query& q) {
    string line;
    getline(is, line);
    auto pos_colon =  line.find(":");  
    auto space_colon =  line.find_first_not_of(" ");  
    request_section = line.substr(0, pos_colon );
    list_section = line.substr(pos_colon + 1);
    if (request_section.substr(0, space_colon) == "Bus"s) {
        vector<string> bus_stops =  SplitStringBySign(list_section, '>');
        q.type = QueryType::Bus;
        q.bus.bus = request_section.substr(space_colon, pos_colon);
        q.bus.stops = bus_stops;
        }
    else if (request_section.substr(0, space_colon) == "Stop"s) {
        q.type = QueryType::Stop;
        q.stop.stop = request_section.substr(space_colon, pos_colon);
        pair<double, double> coordinates =  SplitStringBySign(list_section, ',');
        q.stop.coordinates.lat = coordinates.first;
        q.stop.coordinates.lng = coordinates.second;
    }
    
    else if (pos_colon  == npos) {
        q.type = QueryType::Stop;
        q.stop = request_section.substr(space_colon, pos_colon);
        pair<double, double> coordinates =  SplitStringBySign(list_section, ',');
        q.lat = coordinates.first;
        q.lat = coordinates.second;
    }
    else {return is >> q;} 
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
    }
    }
private:
    deque<pair <string, string>> deq_;
};

