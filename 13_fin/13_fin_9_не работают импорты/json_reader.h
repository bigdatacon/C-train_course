#pragma once

#include <sstream>
#include <string>
#include <deque>
#include <iostream>
#include <vector>

#include "transport_catalogue.h"
#include "domain.h"
#include "json.h"
#include "geo.h"
#include "map_renderer.h"
#include "json_builder.h"

//#include "graph.h"
//#include "router.h"
#include "transport_router.h"


inline json::Document LoadJSON(const std::string& s) {
    std::istringstream strm(s);
    return json::Load(strm);
}

inline std::string Print(const json::Node& node) {
    std::ostringstream out;
    Print(json::Document{ node }, out);
    return out.str();
}


namespace transport_catalogue {

    class InputReaderJson {
    public:
        explicit InputReaderJson(std::istream& is);

        void ReadInputJsonBaseRequest();
        void ReadInputJsonStatRequest();
        void ReadInputJsonRenderSettings();

        void ReadInputJsonRouteSettings();


        void ReadInputJsonRequest();

        void UpdStop(TransportCatalogue& tc);

        void UpdBus(TransportCatalogue& tc);

        void UpdStopDist(TransportCatalogue& tc);

        /*, graph::ActivityProcessor<Weight>& actprocess*/

        template <typename Weight>
        void ManageOutputRequests(TransportCatalogue& tc, MapRenderer& mr, graph::ActivityProcessor<Weight>& actprocess);

        RenderData GetRenderData();

        void UpdRouteSettings(TransportCatalogue& tc);



    private:
        std::istream& is_;

        std::deque<OutputRequest> out_req_;

        std::deque<domain::BusDescription> upd_req_bus_;
        std::deque<domain::Stop> upd_req_stop_;
        std::vector<domain::StopDistancesDescription> distances_;
        RenderData render_data_;
        json::Document load_;
        domain::RouteSettings route_settings_;

    };

}





