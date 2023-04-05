// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "input_reader.h"
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

struct AllBusInfoBusResponse {
    // Наполните полями эту структуру
    string bus;
    int stops;
    int uniq_stops;
    int r_length;
};

class TransportCatalogue {
public:
    void AddBus(const Query q);

    void AddStop(const Query q);

    vector<string> FindBus(const string bus);
    pair<double, double> FindStop(const string stop);

    AllBusInfoBusResponse GetAllBusInfo(const string bus);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/*class TransportCatalogue {
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
        AllBusInfoBusResponse all_r;
        vector<string> stops_v = FindBus(bus);
        all_r.bus = bus;
        all_r.stops = stops_v.size();
        all_r.uniq_stops = countUnique(stops_v);
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
        return all_r;
    }*/
