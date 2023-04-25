#pragma once

#include <sstream>
#include <string>
#include <deque>
#include <iostream>
#include <vector>

#include "transport_catalogue.h"
#include "json.h"
#include "geo.h"


using namespace json;
using namespace std::literals;



inline json::Document LoadJSON(const std::string& s) {
    std::istringstream strm(s);
    return json::Load(strm);
}

inline std::string Print(const Node& node) {
    std::ostringstream out;
    Print(Document{ node }, out);
    return out.str();
}


namespace transport_catalogue {


    class InputReaderJson {
    public:
        explicit InputReaderJson(std::istream& is);

        void ReadInputJsonRequest();

        void UpdStop(TransportCatalogue& tc);

        void UpdBus(TransportCatalogue& tc);

        void UpdStopDist(TransportCatalogue& tc);

        void ManageOutputRequests(TransportCatalogue& tc);



    private:
        std::istream& is_;

        std::deque<OutputRequest> out_req_;

        std::deque<BusDescription> upd_req_bus_;
        std::deque<Stop> upd_req_stop_;
        std::vector<StopDistancesDescription> distances_;


    };

}





