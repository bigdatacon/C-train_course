#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <set>

using namespace std;

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            words.push_back(word);
            word = "";
        }
        else {
            word += c;
        }
    }
    words.push_back(word);

    return words;
}

// метод получения среза вектора 
template<typename T>
std::vector<T> slice(std::vector<T> const& v, int m)
{
    auto first = v.cbegin() + m;
    auto last = v.end();

    std::vector<T> vec(first, last);
    return vec;
}

enum class QueryType {
    NewBus,
    BusesForStop,
    StopsForBus,
    AllBuses,
};

struct Query {
    QueryType type;
    string bus;
    string stop;
    vector<string> stops;
};

istream& operator>>(istream& is, Query& q) {
    // Реализуйте эту функцию
    string line;
    /* Считать строку до знака новой строки */
    getline(is, line);
    vector<string> query_from_cin = SplitIntoWords(line);
    if (query_from_cin[0] == "NEW_BUS"s) {
        vector<string> bus_stops = slice(query_from_cin, 3);
        AddBus(query_from_cin[1], bus_stops)
    }
    else if (query_from_cin[0] == "BusesForStop"s) {
        
        BusesForStopResponse buses_data = GetBusesForStop(query_from_cin[1]);
        return buses_data;
    }

    else if (query_from_cin[0] == "StopsForBus"s) {

        StopsForBusResponse stop_data = GetStopsForBus(query_from_cin[1]);
        return stop_data;
    }

    else if (query_from_cin[0] == "AllBuses"s) {

        AllBusesResponse all_buses_data = GetAllBuses();
        return all_buses_data;
    }

    return is;
}


/*
На запрос BUSES_FOR_STOP stop выведите через пробел список автобусов, проезжающих через эту остановку, в том порядке, в котором они создавались командами NEW_BUS. Если остановка stop не существует, выведите No stop.
На запрос STOPS_FOR_BUS bus выведите описания остановок маршрута bus в отдельных строках в том порядке, в котором они были заданы в соответствующей команде NEW_BUS. Описание каждой остановки stop должно иметь вид Stop stop: bus1 bus2 ...,
*/

struct BusesForStopResponse {
    // Наполните полями эту структуру
    set<string> buses_set;
};

ostream& operator<<(ostream& os, const BusesForStopResponse& r) {
    // Реализуйте эту функцию
    return os;
}

struct StopsForBusResponse {
    // Наполните полями эту структуру
    vector<string> stops;
};

ostream& operator<<(ostream& os, const StopsForBusResponse& r) {
    // Реализуйте эту функцию
    return os;
}

/*
На запрос ALL_BUSES выведите описания всех автобусов в алфавитном порядке. Описание каждого маршрута bus должно иметь вид Bus bus: stop1 stop2 ..., где stop1 stop2 ... — список 
остановок автобуса bus в том порядке, в каком они были заданы в соответствующей команде NEW_BUS. Если автобусы отсутствуют, выведите No buses.
*/

struct AllBusesResponse {
    // Наполните полями эту структуру
    string bus;
   vector<StopsForBusResponse> stop_for_buses;
};

ostream& operator<<(ostream& os, const AllBusesResponse& r) {
    // Реализуйте эту функцию
    return os;
}

class BusManager {
public:
    void AddBus(const string& bus, const vector<string>& stops) {
        // Реализуйте этот метод
        allbusesresponse_[bus] = stops;
    }

    BusesForStopResponse GetBusesForStop(const string& stop) const {
        // Реализуйте этот метод
        set<string> buses_set;
        for (auto [bus, stops] : allbusesresponse_) {
            
            if (stops.count(stop)!=0) { buses_set.insert(bus);  }
        }

        return buses_set;
    }

    StopsForBusResponse GetStopsForBus(const string& bus) const {
        // Реализуйте этот метод
        return  allbusesresponse_[bus];


    }

    AllBusesResponse GetAllBuses() const {

        // Реализуйте этот метод
        if (allbusesresponse_.empty()) {
            cout << "No buses"s << endl;
        }
        else {
            for (const auto& bus_item : stops_to_buses_) {
                cout << "Bus "s << bus_item.bus << ": "s;
                for (const string& stop : bus_item.stops) {
                    cout << stop << " "s;
                }
                cout << endl;


            }
        }
    }

private:
    //map<string, vector<string>>
    BusesForStopResponse    buses_to_stops_;   // stop : vector<bus>
    StopsForBusResponse stops_to_buses_;    // bus : vector<stops>
    AllBusesResponse allbusesresponse_;

};  

// Реализуйте функции и классы, объявленные выше, чтобы эта функция main
// решала задачу "Автобусные остановки"

int main() {
    int query_count;
    Query q;

    cin >> query_count;

    BusManager bm;
    for (int i = 0; i < query_count; ++i) {
        cin >> q;
        switch (q.type) {
        case QueryType::NewBus:
            bm.AddBus(q.bus, q.stops);
            break;
        case QueryType::BusesForStop:
            cout << bm.GetBusesForStop(q.stop) << endl;
            break;
        case QueryType::StopsForBus:
            cout << bm.GetStopsForBus(q.bus) << endl;
            break;
        case QueryType::AllBuses:
            cout << bm.GetAllBuses() << endl;
            break;
        }
    }
}
