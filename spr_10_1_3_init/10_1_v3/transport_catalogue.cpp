#include "input_reader.h"
#include "transport_catalogue.h"

using namespace std;


void TransportCatalogue::AddBus(const UpdateQuery q) {
    buses_[q.bus.bus] = q.bus.stops;
}

void TransportCatalogue::AddStop(const UpdateQuery q) {
    stops_[q.stop.stop] = make_pair(q.stop.coordinates.lat, q.stop.coordinates.lng);
}

vector<string> TransportCatalogue::FindBus(const string bus) {
    vector<string> res;
    if (buses_.count(bus)) { return buses_[bus]; }
    else { cout << "Bus " << bus << ": not found" << endl; return res; }
}

pair<double, double> TransportCatalogue::FindStop(const string stop) {
    pair<double, double> res;
    if (stops_.count(stop)) { return stops_[stop]; }
    else { cout << "Stop " << stop << ": not found" << endl; return res; }
}

AllBusInfoBusResponse TransportCatalogue::GetAllBusInfo(const string bus) {
    AllBusInfoBusResponse all_r;
    vector<string> stops_v = FindBus(bus);
    all_r.bus = bus;
    all_r.stops = stops_v.size();
    all_r.uniq_stops = countUnique(stops_v);
    // ïîäñ÷åò ðàññòîÿíèÿ ComputeDistance
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
}
