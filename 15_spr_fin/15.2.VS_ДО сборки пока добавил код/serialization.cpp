#include "serialization.h"

namespace serialization {

    template <typename It>
    uint32_t calculate_id(It start, It end, std::string_view name) {

        auto stop_it = std::find_if(start, end, [&name](const domain::Stop stop) {return stop.name == name; });
        return std::distance(start, stop_it);
    }

    transport_catalogue_protobuf::TransportCatalogue transport_catalogue_serialization(const transport_catalogue::TransportCatalogue& transport_catalogue) {

        transport_catalogue_protobuf::TransportCatalogue transport_catalogue_proto;

        const auto& stops = transport_catalogue.GetStops(); //const std::deque<Stop>&
        const auto& buses = transport_catalogue.GetBuses(); //const std::deque<Bus>&
        const auto& distances = transport_catalogue.GetStopDistances(); //std::unordered_map<std::pair<const domain::Stop*, const domain::Stop*>, int

        int id = 0;
        for (const auto& stop : stops) {

            transport_catalogue_protobuf::Stop stop_proto;

            stop_proto.set_id(id);
            stop_proto.set_name(stop.stop_name);
            stop_proto.set_latitude(stop.coordinates.lat);
            stop_proto.set_longitude(stop.coordinates.lng);

            *transport_catalogue_proto.add_stops() = std::move(stop_proto);

            ++id;
        }

        for (const auto& bus : buses) {

            transport_catalogue_protobuf::Bus bus_proto;

            bus_proto.set_name(bus.bus_name);

            for (auto stop : bus.stops) {
                uint32_t stop_id = calculate_id(stops.cbegin(),
                    stops.cend(),
                    stop);
                bus_proto.add_stops(stop_id);
            }

            bus_proto.set_is_roundtrip(bus.type);
            domain::AllBusInfoBusResponse allbusresp = transport_catalogue.GetAllBusInfo(bus.bus_name);
            bus_proto.set_route_length(allbusresp.route_length);

            *transport_catalogue_proto.add_buses() = std::move(bus_proto);
        }

        for (const auto& [pair_stops, pair_distance] : distances) {

            transport_catalogue_protobuf::Distance distance_proto;

            distance_proto.set_start(calculate_id(stops.cbegin(),
                stops.cend(),
                pair_stops.first->stop_name));

            distance_proto.set_end(calculate_id(stops.cbegin(),
                stops.cend(),
                pair_stops.second->stop_name));

            distance_proto.set_distance(pair_distance);

            *transport_catalogue_proto.add_distances() = std::move(distance_proto);
        }

        return transport_catalogue_proto;
    }


    transport_catalogue::TransportCatalogue transport_catalogue_deserialization(const transport_catalogue_protobuf::TransportCatalogue& transport_catalogue_proto) {

        transport_catalogue::TransportCatalogue transport_catalogue;

        const auto& stops_proto = transport_catalogue_proto.stops();
        const auto& buses_proto = transport_catalogue_proto.buses();
        const auto& distances_proto = transport_catalogue_proto.distances();

        for (const auto& stop : stops_proto) {

            domain::Stop tc_stop;

            tc_stop.stop_name = stop.name();
            tc_stop.coordinates.lat= stop.latitude();
            tc_stop.coordinates.lng = stop.longitude();

            transport_catalogue.AddStop(std::move(tc_stop));
        }

        const auto& tc_stops = transport_catalogue.GetStops(); //const std::deque<Stop>& 

        std::vector<domain::Distance> distances;
        for (const auto& distance : distances_proto) {

            domain::Distance tc_distance;

            tc_distance.start = transport_catalogue.FindStop(tc_stops[distance.start()].name);
            tc_distance.end = transport_catalogue.FindStop(tc_stops[distance.end()].name);

            tc_distance.distance = distance.distance();

            distances.push_back(tc_distance);
        }

        transport_catalogue.AddDistanceFromSerializer(distances);

        for (const auto& bus_proto : buses_proto) {

            domain::Bus tc_bus;

            tc_bus.bus_name = bus_proto.name();

            for (auto stop_id : bus_proto.stops()) {
                auto name = tc_stops[stop_id].name;
                tc_bus.stops.push_back(transport_catalogue.FindStop(name)->stop_name);
            }

            tc_bus.type = bus_proto.is_roundtrip();
            //tc_bus.route_length = bus_proto.route_length();  // ƒлина маршрута не добавл€ю так как она отдельно считаелась всегда от остановок

            transport_catalogue.AddBusFromSerializer(std::move(tc_bus));
        }

        return transport_catalogue;
    }

    void catalogue_serialization(transport_catalogue::TransportCatalogue& transport_catalogue, std::ostream& out) {

        transport_catalogue_protobuf::TransportCatalogue transport_catalogue_proto = transport_catalogue_serialization(transport_catalogue);
        transport_catalogue_proto.SerializePartialToOstream(&out);
    }

    transport_catalogue::TransportCatalogue catalogue_deserialization(std::istream& in) {

        transport_catalogue_protobuf::TransportCatalogue transport_catalogue_proto;
        auto success_parsing_catalogue_from_istream = transport_catalogue_proto.ParseFromIstream(&in);

        if (!success_parsing_catalogue_from_istream) {
            throw std::runtime_error("cannot parse serialized file from istream");
        }

        return transport_catalogue_deserialization(transport_catalogue_proto);
    }

}//end namespace serialization