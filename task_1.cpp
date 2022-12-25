#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

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
    //Query query;
    if (query_from_cin[0] == "NEW_BUS"s) {
        vector<string> bus_stops = slice(query_from_cin, 3);
        q.type = QueryType::NewBus;
        q.bus = query_from_cin[1];
        q.stop = query_from_cin[2];
        q.stops = bus_stops;
    }
    else if (query_from_cin[0] == "BUSES_FOR_STOP"s) {
        q.type = QueryType::BusesForStop;
        //query.bus = query_from_cin[1];
        q.stop = query_from_cin[1];
        //query.stops = bus_stops;
    }

    else if (query_from_cin[0] == "STOPS_FOR_BUS"s) {
        q.type = QueryType::StopsForBus;
        q.bus = query_from_cin[1];
        //query.stop = query_from_cin[1];
        //query.stops = bus_stops;
    }

    else if (query_from_cin[0] == "ALL_BUSES"s) {
      q.type = QueryType::AllBuses;
    } else if (query_from_cin[0] == ""s) {
        return is >> q;
    }
    return is;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////// Вспомогательыне функции/инфо закончились

struct BusesForStopResponse {
    // Наполните полями эту структуру
    vector<string> buses;
};



struct StopsForBusResponse {
    // Наполните полями эту структуру
	vector<pair<string, vector<string>>> stops_and_bus;
};


struct AllBusesResponse {
    // Наполните полями эту структуру
    //string bus;
    //StopsForBusResponse stop_for_buses;
    //vector<string> stop_for_buses;
    map<string, vector<string>> stop_for_buses;
};


ostream& operator<<(ostream &os, const BusesForStopResponse &r) {
	// Реализуйте эту функцию
	//cout << "BusesForStopResponse"s << endl;
	if (r.buses.empty()) {
		cout << "No stop";
	} else {
		int size_v = r.buses.size();
		int sch = 1;
		for (auto el : r.buses) {
			if (sch < size_v) {
				cout /*<< "bus : "s */<< el << " ";
			} else {
				cout << el;
			}
			++sch;
		}

	}
	return os;
}



ostream& operator<<(ostream &os, const StopsForBusResponse &r) {
	// Реализуйте эту функцию
	//cout << "StopsForBusResponse"s << endl;
	if (r.stops_and_bus.empty()){cout << "No bus";}
	else {
		bool first = true;
	for (auto [k, v] : r.stops_and_bus) {

	    if (!first) {
	      cout << endl;
	    }
	    first = false;

		if (!v.empty()){

		cout << "Stop "s << k << ": "s;
	      int size_v = v.size();
	      int sch = 1;
		for (auto exempl : v) {
	    	//cout << "sch_stop :"s << sch << "size_v_stop :"s << size_v << endl;
	    	if (sch<size_v) {
			cout /*<< " bus : "s*/ << exempl << " "s;}
	    	else {cout<< exempl;}
	    	++sch;
		}
		}
		else {cout << "Stop "s<< k << ": no interchange"s;}
		//cout << endl;
	}
	;
	}
	return os;
}

//Bus 272: Vnukovo Moskovsky Rumyantsevo Troparyovo

ostream& operator<<(ostream &os, const AllBusesResponse &r) {
  // Реализуйте эту функцию
  //cout << "AllBusesResponse"s << endl;

  if (r.stop_for_buses.empty()){
    cout << "No buses";
  } else {
    bool first = true;
    for (auto [k, v] : r.stop_for_buses) {
      if (!first) {
        cout << endl;
      }
      first = false;
      cout << "Bus "s << k << ": "s;
      int size_v = v.size();
      int sch = 1;

      for (auto exempl : v) {
    	//cout << "sch :"s << sch << "size_v :"s << size_v << endl;
    	if (sch<size_v) {
        cout << exempl << " "s;}
    	else {cout << exempl;}
        ++sch;
      }
    }
  }
  return os;
}



class BusManager {
public:
    void AddBus(const string& bus, const vector<string>& stops) {
        // Реализуйте этот метод
    	AllBusesResponse element;
    	//element.bus = bus;
    	//element.stop_for_buses = stops;

    	//allbusesresponse_.push_back(element.bus, element.stop_for_buses);
    	allbusesresponse_.stop_for_buses.emplace(bus, stops);

    }

    BusesForStopResponse GetBusesForStop(const string& stop) const {
        // Реализуйте этот метод
        //set<string> buses_set;
        BusesForStopResponse buses_set;
        for (auto el : allbusesresponse_.stop_for_buses) {

            if (/*stops.stop_for_buses.count(stop)!=0 */ IsInstanceVec_(el.second, stop)) {buses_set.buses.push_back(el.first);}
        }
        return buses_set;
    }


    //vector<pair<string, vector<string>>> stops_and_bus;
	StopsForBusResponse GetStopsForBus(const string &bus) const {
		// Реализуйте этот метод
		StopsForBusResponse empty_struct;

		if (allbusesresponse_.stop_for_buses.count(bus) > 0) {
			vector<string> stop_for_buses_into = allbusesresponse_.stop_for_buses.at(bus);
			for (string str : stop_for_buses_into) {
				BusesForStopResponse bus_for_next_stop = GetBusesForStop(str);
				eliminateZeroes(bus_for_next_stop, bus);
				if (bus_for_next_stop.buses.size()!=0) {
					empty_struct.stops_and_bus.push_back(
							make_pair(str, bus_for_next_stop.buses));
				}
				else {
					vector<string> empty_vec;
					empty_struct.stops_and_bus.push_back(
						make_pair(str, empty_vec));}
				//continue;

			}

		}
		return empty_struct;
	}


    AllBusesResponse GetAllBuses() const {
        return allbusesresponse_;
    }

private:
    AllBusesResponse allbusesresponse_;
    bool IsInstanceVec_(const vector<string> stops, string el) const {
    	for (auto str : stops){if (str==el) {return true;}
    	}
    	return false;
    }

    BusesForStopResponse & eliminateZeroes( BusesForStopResponse &answers , string el ) const
    {

        answers.buses.erase(remove( answers.buses.begin(), answers.buses.end(), el ), answers.buses.end() );

        return answers;
    }


};
////
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
