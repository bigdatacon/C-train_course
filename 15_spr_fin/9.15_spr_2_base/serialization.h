#pragma once

#include "transport_catalogue.h"
#include "transport_catalogue.pb.h"

#include <iostream>

namespace serialization {

    struct SerializationSettings { std::string file_name; };

    template <typename It>
    uint32_t calculate_id(It start, It end, std::string_view name);

    transport_catalogue_protobuf::TransportCatalogue transport_catalogue_serialization(const transport_catalogue::TransportCatalogue& transport_catalogue);
    transport_catalogue::TransportCatalogue transport_catalogue_deserialization(const transport_catalogue_protobuf::TransportCatalogue& transport_catalogue_proto);

    void catalogue_serialization(transport_catalogue::TransportCatalogue& transport_catalogue, std::ostream& out);
    transport_catalogue::TransportCatalogue catalogue_deserialization(std::istream& in);

}//end namespace serialization