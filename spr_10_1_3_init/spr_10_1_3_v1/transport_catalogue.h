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

class TransportCatalogue {
public:
    void AddBus(const Query q) {
    	buses_[q.bus.bus] = q.bus.stops;
    }
    
    void AddStop(const Query q) {
    	stops_.[q.stop.stop] = std::make_pair(q.stop.lat, q.stop.long);
    }
    

private:
    unordered_map<string , vecror<string>> buses_;
    unordered_map<string , pair<double, double>> stops_;
    
};