#include "serialization.h"

namespace serialization {

    template <typename It>
    uint32_t calculate_id(It start, It end, std::string_view name) {

        auto stop_it = std::find_if(start, end, [&name](const domain::Stop stop) {return stop.stop_name == name; });
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

            tc_distance.start = transport_catalogue.FindStop(tc_stops[distance.start()].stop_name);
            tc_distance.end = transport_catalogue.FindStop(tc_stops[distance.end()].stop_name);

            tc_distance.distance = distance.distance();

            distances.push_back(tc_distance);
        }

        transport_catalogue.AddDistanceFromSerializer(distances);
        /*
        for (const auto& bus_proto : buses_proto) {

            domain::Bus tc_bus;

            tc_bus.bus_name = bus_proto.name();

            for (auto stop_id : bus_proto.stops()) {
                auto name = tc_stops[stop_id].stop_name;
                tc_bus.stops.push_back(transport_catalogue.FindStop(name)->stop_name);
            }

            tc_bus.type = bus_proto.is_roundtrip();
            //tc_bus.route_length = bus_proto.route_length();  // ����� �������� �� �������� ��� ��� ��� �������� ���������� ������ �� ���������

            transport_catalogue.AddBusFromSerializer(std::move(tc_bus));
        }
        */
        for (const auto& bus_proto : buses_proto) {

            domain::BusDescription tc_bus;

            tc_bus.bus_name = bus_proto.name();

            for (auto stop_id : bus_proto.stops()) {
                auto name = tc_stops[stop_id].stop_name;
                tc_bus.stops.push_back(transport_catalogue.FindStop(name)->stop_name);
            }

            tc_bus.type = bus_proto.is_roundtrip();
            //tc_bus.route_length = bus_proto.route_length();  // ����� �������� �� �������� ��� ��� ��� �������� ���������� ������ �� ���������

            //transport_catalogue.AddBus(std::move(tc_bus));
            transport_catalogue.AddBus(tc_bus);
        }

        return transport_catalogue;
    }

    transport_catalogue_protobuf::Color serialize_color(const svg::Color& tc_color) {

        transport_catalogue_protobuf::Color color_proto;

        if (std::holds_alternative<std::monostate>(tc_color)) {
            color_proto.set_none(true);

        } else if (std::holds_alternative<svg::Rgb>(tc_color)) {
            svg::Rgb rgb = std::get<svg::Rgb>(tc_color);

            color_proto.mutable_rgb()->set_red(rgb.red);
            color_proto.mutable_rgb()->set_green(rgb.green);
            color_proto.mutable_rgb()->set_blue(rgb.blue);

        } else if (std::holds_alternative<svg::Rgba>(tc_color)) {
            svg::Rgba rgba = std::get<svg::Rgba>(tc_color);

            color_proto.mutable_rgba()->set_red(rgba.red);
            color_proto.mutable_rgba()->set_green(rgba.green);
            color_proto.mutable_rgba()->set_blue(rgba.blue);
            color_proto.mutable_rgba()->set_opacity(rgba.opacity);

        } else if (std::holds_alternative<std::string>(tc_color)) {
            color_proto.set_string_color(std::get<std::string>(tc_color));
        }

        return color_proto;
    }

    svg::Color deserialize_color(const transport_catalogue_protobuf::Color& color_proto) {

        svg::Color color;

        if (color_proto.has_rgb()) {
            svg::Rgb rgb;

            rgb.red = color_proto.rgb().red();
            rgb.green = color_proto.rgb().green();
            rgb.blue = color_proto.rgb().blue();

            color = rgb;

        } else if (color_proto.has_rgba()) {
            svg::Rgba rgba;

            rgba.red = color_proto.rgba().red();
            rgba.green = color_proto.rgba().green();
            rgba.blue = color_proto.rgba().blue();
            rgba.opacity = color_proto.rgba().opacity();

            color = rgba;

        } else {color = color_proto.string_color();}

        return color;
    }

    transport_catalogue_protobuf::RenderData render_settings_serialization(const transport_catalogue::RenderData& render_settings) {

        transport_catalogue_protobuf::RenderData render_settings_proto;

        render_settings_proto.set_width(render_settings.width);
        render_settings_proto.set_height(render_settings.height);
        render_settings_proto.set_padding(render_settings.padding);
        render_settings_proto.set_line_width(render_settings.line_width);
        render_settings_proto.set_stop_radius(render_settings.stop_radius);
        render_settings_proto.set_bus_label_font_size(render_settings.bus_label_font_size);

        transport_catalogue_protobuf::Point bus_label_offset_proto;
        bus_label_offset_proto.set_x(render_settings.bus_label_offset.first);
        bus_label_offset_proto.set_y(render_settings.bus_label_offset.second);

        *render_settings_proto.mutable_bus_label_offset() = std::move(bus_label_offset_proto);

        render_settings_proto.set_stop_label_font_size(render_settings.stop_label_font_size);

        transport_catalogue_protobuf::Point stop_label_offset_proto;
        stop_label_offset_proto.set_x(render_settings.stop_label_offset.first);
        stop_label_offset_proto.set_y(render_settings.stop_label_offset.second);

        *render_settings_proto.mutable_stop_label_offset() = std::move(stop_label_offset_proto);
        *render_settings_proto.mutable_underlayer_color() = std::move(serialize_color(render_settings.underlayer_color));
        render_settings_proto.set_underlayer_width(render_settings.underlayer_width);

        const auto& colors = render_settings.color_palette;
        for (const auto& color : colors) {
            *render_settings_proto.add_color_palette() = std::move(serialize_color(color));
        }

        return render_settings_proto;
    }

    transport_catalogue::RenderData render_settings_deserialization(const transport_catalogue_protobuf::RenderData& render_settings_proto) {

        transport_catalogue::RenderData render_settings;

        render_settings.width = render_settings_proto.width();
        render_settings.height = render_settings_proto.height();
        render_settings.padding = render_settings_proto.padding();
        render_settings.line_width = render_settings_proto.line_width();
        render_settings.stop_radius = render_settings_proto.stop_radius();
        render_settings.bus_label_font_size = render_settings_proto.bus_label_font_size();

        render_settings.bus_label_offset.first = render_settings_proto.bus_label_offset().x();
        render_settings.bus_label_offset.second = render_settings_proto.bus_label_offset().y();

        render_settings.stop_label_font_size = render_settings_proto.stop_label_font_size();

        render_settings.stop_label_offset.first = render_settings_proto.stop_label_offset().x();
        render_settings.stop_label_offset.second = render_settings_proto.stop_label_offset().y();

        render_settings.underlayer_color = deserialize_color(render_settings_proto.underlayer_color());
        render_settings.underlayer_width = render_settings_proto.underlayer_width();

        for (const auto& color_proto : render_settings_proto.color_palette()) {
            render_settings.color_palette.push_back(deserialize_color(color_proto));
        }

        return render_settings;
    }

    void catalogue_serialization(transport_catalogue::TransportCatalogue& transport_catalogue, transport_catalogue::RenderData& render_settings, std::ostream& out) {

        transport_catalogue_protobuf::Catalogue catalogue_proto;

        transport_catalogue_protobuf::TransportCatalogue transport_catalogue_proto = transport_catalogue_serialization(transport_catalogue);
        transport_catalogue_protobuf::RenderData render_settings_proto = render_settings_serialization(render_settings);

        *catalogue_proto.mutable_transport_catalogue() = std::move(transport_catalogue_proto);
        *catalogue_proto.mutable_render_settings() = std::move(render_settings_proto);

        catalogue_proto.SerializePartialToOstream(&out);

    }

    Catalogue catalogue_deserialization(std::istream& in) {

        transport_catalogue_protobuf::Catalogue catalogue_proto;
        auto success_parsing_catalogue_from_istream = catalogue_proto.ParseFromIstream(&in);

        if (!success_parsing_catalogue_from_istream) {
            throw std::runtime_error("cannot parse serialized file from istream");
        }

        return {transport_catalogue_deserialization(catalogue_proto.transport_catalogue()),
                render_settings_deserialization(catalogue_proto.render_settings())};
    }


}//end namespace serialization