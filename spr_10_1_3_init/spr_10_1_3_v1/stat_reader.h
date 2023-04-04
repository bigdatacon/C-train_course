// напишите решение с нуля
// код сохраните в свой git-репозиторий
// напишите решение с нуля
// код сохраните в свой git-репозиторий


//AllBusInfoBusResponse
/*
struct AllBusInfoBusResponse {
    // Наполните полями эту структуру
	    int stops;
        int uniq_stops;
        int r_length ;
};
*/

class Stat_reader {
public:
    istream& operator>>(istream& is, Query& q, TransportCatalogue tc) {
    int query_count;
    cin >> query_count;

    for (int i = 0; i < query_count; ++i) {
    string line;
    // После заполнения базы читаю и записываю запросы на вывод 
    while (std::getline(std::cin, line)) {
        auto space_colon =  line.find_first_not_of(" ");  // : отделяет название запроса
        string type_req   = line.substr(0, space_colon);
        string number   = line.substr(space_colon);
        deq_.push_back(std::make_pair(type_req, number));        
    }     
    }
        
        
    ostream& operator<<(ostream &os, const AllBusInfoBusResponse &r) {
	// Реализуйте эту функцию
	//cout << "BusesForStopResponse"s << endl;
	if (r.stops == ) {
		cout << "No stop for Bus";
	} else {
        cout << "Bus " << bus << ":"s << stops << " stops on route, "s << uniq_stops << " unique stops, "s <<r_length << " route length"s  << endl;
	}
	return os;
}
        
    void Output(){
        for (const auto& element : deq_) {
        if (element.first == "Bus"s) {cout <<tc.GetAllBusInfo; } 
        }
        break;

    }
        
        
private:
    deque<pair <string, string>> deq_; // тут перечень запросов на вывод 
};
