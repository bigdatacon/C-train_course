#pragma once

#include "transport_catalogue.h"
#include "transport_catalogue.pb.h"

#include "svg.pb.h"

#include "map_renderer.h"
#include "map_renderer.pb.h"

#include <iostream>

namespace serialization {

    struct SerializationSettings {std::string file_name;};

    struct Catalogue {
        transport_catalogue::TransportCatalogue transport_catalogue_;
        transport_catalogue::RenderSettings render_settings_;
    };

    template <typename It>
    uint32_t calculate_id(It start, It end, std::string_view name);

    transport_catalogue_protobuf::TransportCatalogue transport_catalogue_serialization(const transport_catalogue::TransportCatalogue& transport_catalogue);
    transport_catalogue::TransportCatalogue transport_catalogue_deserialization(const transport_catalogue_protobuf::TransportCatalogue& transport_catalogue_proto);

    transport_catalogue_protobuf::Color serialize_color(const svg::Color& tc_color);
    svg::Color deserealize_color(const transport_catalogue_protobuf::Color& color_proto);
    transport_catalogue_protobuf::RenderSettings render_settings_serialization(const transport_catalogue::RenderSettings& render_settings);
    transport_catalogue::RenderSettings render_settings_deserialization(const transport_catalogue_protobuf::RenderSettings& render_settings_proto);

    void catalogue_serialization(transport_catalogue::TransportCatalogue& transport_catalogue,
                                 transport_catalogue::RenderSettings& render_settings,
                                 std::ostream& out);

    Catalogue catalogue_deserialization(std::istream& in);

}//end namespace serialization