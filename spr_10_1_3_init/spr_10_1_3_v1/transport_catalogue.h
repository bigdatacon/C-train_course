// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "input_reader.h"

/*
enum class Stop {
    string stop,
    double lat,
    double long
};

enum class Bus {
    string bus,
    vector<string> stops,
    //bool circle
};

struct Query {
    QueryType type;
    Bus bus;
    Stop stop;
};
*/

/*struct Coordinates {
    double lat;
    double lng;
    bool operator==(const Coordinates& other) const {
        return lat == other.lat && lng == other.lng;
    }
    bool operator!=(const Coordinates& other) const {
        return !(*this == other);
    }
};*/

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
