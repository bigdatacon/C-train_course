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


ostream& operator<<(ostream& os,  const AllBusInfoBusResponse& r) {
    // Реализуйте эту функцию
    //cout << "BusesForStopResponse"s << endl;
    if (r.stops == 0 ) {
        cout << "No stop for Bus";
    }
    else {
        cout << "Bus " << r.bus << ":"s << r.stops << " stops on route, "s << r.uniq_stops << " unique stops, "s << r.r_length << " route length"s << endl;
    }
    return os;
}

class Stat_reader {
public:
    istream& operator>>(istream& is, Query& q, TransportCatalogue tc) {
    int query_count;
    cin >> query_count;
    for (int j = 0; j < query_count; ++j) {
        string line;
        getline(std::cin, line);
        if (!line.empty()) {
                auto space_colon = line.find(" ");  // : отделяет название запроса
                string type_req = line.substr(0, space_colon);
                string number = line.substr(space_colon+1);
                deq_.push_back(std::make_pair(type_req, number));
            }
        }
        
    void Output(){
        for (const auto& element : deq_) {
        if (element.first == "Bus"s) {cout <<tc.GetAllBusInfo(element.second); } 
        }
        break;

    }
        
        
private:
    deque<pair <string, string>> deq_; // тут перечень запросов на вывод 
};
