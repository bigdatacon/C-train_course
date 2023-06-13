-----------------------------------------------------------------------
main.cpp
-----------------------------------------------------------------------
#include <fstream>
#include <iostream>

#include "json_reader.h"
#include "request_handler.h"

using namespace std;

using namespace transport_catalogue;
using namespace transport_catalogue::detail::json;
using namespace transport_catalogue::detail::router;

using namespace map_renderer;
using namespace request_handler;
using namespace serialization;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);
    
    TransportCatalogue transport_catalogue;   
    
    RenderSettings render_settings;
    RoutingSettings routing_settings;
    
    SerializationSettings serialization_settings;
    
    JSONReader json_reader;
    vector<StatRequest> stat_request;   
    
    if (mode == "make_base"sv) {
        
        json_reader = JSONReader(cin); 
        
        json_reader.parse_node_make_base(transport_catalogue, 
                                         render_settings, 
                                         routing_settings, 
                                         serialization_settings);
        
        ofstream out_file(serialization_settings.file_name, ios::binary);    
        catalogue_serialization(transport_catalogue, render_settings, out_file);
        
    } else if (mode == "process_requests"sv) {
        
        json_reader = JSONReader(cin);    
        
        json_reader.parse_node_process_requests(stat_request, 
                                                serialization_settings);
        
        ifstream in_file(serialization_settings.file_name, ios::binary); 
        
        Catalogue catalogue = catalogue_deserialization(in_file);
            
        RequestHandler request_handler;       
        
        request_handler.execute_queries(catalogue.transport_catalogue_, 
                                        stat_request, 
                                        catalogue.render_settings_,
                                        routing_settings);
        
        print(request_handler.get_document(), cout); 
        
    } else {
        PrintUsage();
        return 1;
    }
}
-----------------------------------------------------------------------
CMakeLists.txt
-----------------------------------------------------------------------
cmake_minimum_required(VERSION 3.12)

project(final_project_15)
set(CMAKE_CXX_STANDARD 17)

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS transport_catalogue.proto 
                                            svg.proto 
                                            map_renderer.proto)

set(UTILITY geo.h 
            geo.cpp 
            ranges.h)

set(TRANSPORT_CATALOGUE domain.h 
                        domain.cpp
                        transport_catalogue.h 
                        transport_catalogue.cpp 
                        transport_catalogue.proto)
                      
set(ROUTER graph.h 
           router.h        
           transport_router.h 
           transport_router.cpp)
                              
set(JSON json.h 
         json.cpp 
         json_builder.h 
         json_builder.cpp 
         json_reader.h 
         json_reader.cpp)
               
set(SVG svg.h 
        svg.cpp
        svg.proto)
        
set(MAP_RENDERER map_renderer.h
                 map_renderer.cpp
                 map_renderer.proto)
              
set(SERIALIZATION serialization.h 
                  serialization.cpp)
                 
set(REQUEST_HANDLER request_handler.h 
                    request_handler.cpp)

add_executable(transport_catalogue main.cpp 
                                   ${PROTO_SRCS} 
                                   ${PROTO_HDRS} 
                                   ${UTILITY}
                                   ${TRANSPORT_CATALOGUE}
                                   ${ROUTER} 
                                   ${JSON}
                                   ${SVG} 
                                   ${MAP_RENDERER} 
                                   ${SERIALIZATION}
                                   ${REQUEST_HANDLER})

target_include_directories(transport_catalogue PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(transport_catalogue PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(transport_catalogue "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}>" Threads::Threads)
-----------------------------------------------------------------------
domain.cpp
-----------------------------------------------------------------------

empty

-----------------------------------------------------------------------
domain.h
-----------------------------------------------------------------------
#pragma once

#include <algorithm>
#include <vector>
#include <string>
#include <variant>

#include "geo.h"
#include "graph.h"

namespace domain {

struct StatRequest { 
    int id;
    std::string type;
    std::string name;    
    std::string from;
    std::string to;
};
    
struct Bus;
    
struct Stop {     
    std::string name;
    double latitude;
    double longitude;
    
    std::vector<Bus*> buses;
};

struct Bus {     
    std::string name;
    std::vector<Stop*> stops;
    bool is_roundtrip;
    size_t route_length;
};

struct Distance {    
    const Stop* start;
    const Stop* end;
    int distance;
};  

struct BusQueryResult {
    std::string_view name;
    bool not_found;
    int stops_on_route;
    int unique_stops;
    int route_length;
    double curvature;
};    
   
struct StopQueryResult {
    std::string_view name;
    bool not_found;
    std::vector<std::string> buses_name;
};
    
struct StopEdge {
	std::string_view name;
	double time = 0;
};

struct BusEdge {
	std::string_view bus_name;
	size_t span_count = 0;
	double time = 0;
};

struct RoutingSettings {
	double bus_wait_time = 0;
	double bus_velocity = 0;
};

struct RouterByStop {
	graph::VertexId bus_wait_start;
	graph::VertexId bus_wait_end;
};

struct RouteInfo {
	double total_time = 0.;
	std::vector<std::variant<StopEdge, BusEdge>> edges;
};

}//end namespace domain
-----------------------------------------------------------------------
geo.cpp
-----------------------------------------------------------------------
#define _USE_MATH_DEFINES
#include "geo.h"

#include <cmath>

namespace geo {
    
double compute_distance(Coordinates start, Coordinates end) {
    using namespace std;
    
    if (!(start == end)) {
        const double dr = PI / 180.;
        return acos(sin(start.latitude * dr) * sin(end.latitude * dr)
                    + cos(start.latitude * dr) * cos(end.latitude * dr) 
                    * cos(abs(start.longitude - end.longitude) * dr)) * EARTH_RADIUS;
    } else {
        return 0.0;
    }
}
    
}//end namespace geo
-----------------------------------------------------------------------
geo.h
-----------------------------------------------------------------------
#pragma once
#include <cmath>

namespace geo {
    
const int EARTH_RADIUS = 6371000;
const double PI = 3.1415926535;
    
struct Coordinates {
    double latitude;
    double longitude;

    bool operator==(const Coordinates& other) const {
        return latitude == other.latitude && longitude == other.longitude;
    }
    bool operator!=(const Coordinates& other) const {
        return !(*this == other);
    }
};
    
double compute_distance(Coordinates start, Coordinates end);
    
}//end namespace geo
-----------------------------------------------------------------------
graph.h
-----------------------------------------------------------------------
#pragma once

#include "ranges.h"

#include <cstdlib>
#include <vector>

namespace graph {

using VertexId = size_t;
using EdgeId = size_t;

template <typename Weight>
struct Edge {
    VertexId from;
    VertexId to;
    Weight weight;
};

template <typename Weight>
class DirectedWeightedGraph {
private:
    using IncidenceList = std::vector<EdgeId>;
    using IncidentEdgesRange = ranges::Range<typename IncidenceList::const_iterator>;

public:
    DirectedWeightedGraph() = default;
    explicit DirectedWeightedGraph(size_t vertex_count);
    EdgeId add_edge(const Edge<Weight>& edge);

    size_t get_vertex_count() const;
    size_t get_edge_count() const;
    const Edge<Weight>& get_edge(EdgeId edge_id) const;
    IncidentEdgesRange get_incident_edges(VertexId vertex) const;

private:
    std::vector<Edge<Weight>> edges_;
    std::vector<IncidenceList> incidence_lists_;
};

template <typename Weight>
DirectedWeightedGraph<Weight>::DirectedWeightedGraph(size_t vertex_count) : incidence_lists_(vertex_count) {}

template <typename Weight>
EdgeId DirectedWeightedGraph<Weight>::add_edge(const Edge<Weight>& edge) {
    edges_.push_back(edge);
    
    const EdgeId id = edges_.size() - 1;
    
    incidence_lists_.at(edge.from).push_back(id);
    
    return id;
}

template <typename Weight>
size_t DirectedWeightedGraph<Weight>::get_vertex_count() const {return incidence_lists_.size();}

template <typename Weight>
size_t DirectedWeightedGraph<Weight>::get_edge_count() const {return edges_.size();}

template <typename Weight>
const Edge<Weight>& DirectedWeightedGraph<Weight>::get_edge(EdgeId edge_id) const {return edges_.at(edge_id);}

template <typename Weight>
typename DirectedWeightedGraph<Weight>::IncidentEdgesRange DirectedWeightedGraph<Weight>::get_incident_edges(VertexId vertex) const {
    return ranges::as_range(incidence_lists_.at(vertex));
}

}//end namespace graph
-----------------------------------------------------------------------
json_builder.cpp
-----------------------------------------------------------------------
#include "json_builder.h"

namespace transport_catalogue {
namespace detail {
namespace json {
namespace builder {

BaseContext::BaseContext(Builder& builder) : builder_(builder) {}

KeyContext BaseContext::key(const std::string& key) {return builder_.key(key);}
Builder& BaseContext::value(const Node::Value& value) {return builder_.value(value);}

DictionaryContext BaseContext::start_dict() {return DictionaryContext(builder_.start_dict());}
Builder& BaseContext::end_dict() {return builder_.end_dict();}

ArrayContext BaseContext::start_array() {return ArrayContext(builder_.start_array());}
Builder& BaseContext::end_array() {return builder_.end_array();}

KeyContext::KeyContext(Builder& builder) : BaseContext(builder) {}

DictionaryContext KeyContext::value(const Node::Value& value) {return BaseContext::value(std::move(value));}

DictionaryContext::DictionaryContext(Builder& builder) : BaseContext(builder) {}

ArrayContext::ArrayContext(Builder& builder) : BaseContext(builder) {}

ArrayContext ArrayContext::value(const Node::Value& value) {return BaseContext::value(std::move(value));}

Node Builder::make_node(const Node::Value& value_) {
    Node node;

    if (std::holds_alternative<bool>(value_)) {
        bool bol = std::get<bool>(value_);
        node = Node(bol);

    } else if (std::holds_alternative<int>(value_)) {
        int intt = std::get<int>(value_);
        node = Node(intt);

    } else if (std::holds_alternative<double>(value_)) {
        double doble = std::get<double>(value_);
        node = Node(doble);

    } else if (std::holds_alternative<std::string>(value_)) {
        std::string str = std::get<std::string>(value_);
        node = Node(std::move(str));

    } else if (std::holds_alternative<Array>(value_)) {
        Array arr = std::get<Array>(value_);
        node = Node(std::move(arr));

    } else if (std::holds_alternative<Dict>(value_)) {
        Dict dictionary = std::get<Dict>(value_);
        node = Node(std::move(dictionary));

    } else {
        node = Node();
    }

    return node;
}

void Builder::add_node(const Node& node) {
    if (nodes_stack_.empty()) {

        if (!root_.is_null()) {
            throw std::logic_error("root has been added");
        }

        root_ = node;
        return;

    } else {

        if (!nodes_stack_.back()->is_array()
            && !nodes_stack_.back()->is_string()) {

            throw std::logic_error("unable to create node");
        }

        if (nodes_stack_.back()->is_array()) {
            Array arr = nodes_stack_.back()->as_array();
            arr.emplace_back(node);

            nodes_stack_.pop_back();
            auto arr_ptr = std::make_unique<Node>(arr);
            nodes_stack_.emplace_back(std::move(arr_ptr));

            return;
        }

        if (nodes_stack_.back()->is_string()) {
            std::string str = nodes_stack_.back()->as_string();
            nodes_stack_.pop_back();

            if (nodes_stack_.back()->is_dict()) {
                Dict dictionary = nodes_stack_.back()->as_dict();
                dictionary.emplace(std::move(str), node);

                nodes_stack_.pop_back();
                auto dictionary_ptr = std::make_unique<Node>(dictionary);
                nodes_stack_.emplace_back(std::move(dictionary_ptr));
            }

            return;
        }
    }
}

KeyContext Builder::key(const std::string& key_) {
    if (nodes_stack_.empty()) {
        throw std::logic_error("unable to create key");
    }

    auto key_ptr = std::make_unique<Node>(key_);

    if (nodes_stack_.back()->is_dict()) {
        nodes_stack_.emplace_back(std::move(key_ptr));
    }

    return KeyContext(*this);
}

Builder& Builder::value(const Node::Value& value_) {
    add_node(make_node(value_));

    return *this;
}

DictionaryContext Builder::start_dict() {
    nodes_stack_.emplace_back(std::move(std::make_unique<Node>(Dict())));

    return DictionaryContext(*this);
}

Builder& Builder::end_dict() {
    if (nodes_stack_.empty()) {
        throw std::logic_error("unable to close as without opening");
    }

    Node node = *nodes_stack_.back();

    if (!node.is_dict()) {
        throw std::logic_error("object isn't dictionary");
    }

    nodes_stack_.pop_back();
    add_node(node);

    return *this;
}

ArrayContext Builder::start_array() {
    nodes_stack_.emplace_back(std::move(std::make_unique<Node>(Array())));

    return ArrayContext(*this);
}

Builder& Builder::end_array() {
    if (nodes_stack_.empty()) {
        throw std::logic_error("unable to close without opening");
    }

    Node node = *nodes_stack_.back();

    if (!node.is_array()) {
        throw std::logic_error("object isn't array");
    }

    nodes_stack_.pop_back();
    add_node(node);

    return *this;
}

Node Builder::build() {
    if (root_.is_null()) {
        throw std::logic_error("empty json");
    }

    if (!nodes_stack_.empty()) {
        throw std::logic_error("invalid json");
    }

    return root_;
}

}//end namespace builder
}//end namespace json
}//end namespace detail
}//end namespace transport_catalogue
-----------------------------------------------------------------------
json_builder.h
-----------------------------------------------------------------------
#pragma once

#include "json.h"
#include <stack>
#include <string>
#include <memory>

namespace transport_catalogue {
namespace detail {
namespace json {
namespace builder {

class KeyContext;
class DictionaryContext;
class ArrayContext;

class Builder {
public:
    Node make_node(const Node::Value& value_);
    void add_node(const Node& node);

    KeyContext key(const std::string& key_);
    Builder& value(const Node::Value& value);
    
    DictionaryContext start_dict();
    Builder& end_dict();
    
    ArrayContext start_array();
    Builder& end_array();

    Node build();

private:
    Node root_;
    std::vector<std::unique_ptr<Node>> nodes_stack_;
};

class BaseContext {
public:
    BaseContext(Builder& builder);

    KeyContext key(const std::string& key);
    Builder& value(const Node::Value& value);
    
    DictionaryContext start_dict();
    Builder& end_dict();
    
    ArrayContext start_array();
    Builder& end_array();

protected:
    Builder& builder_;
};

class KeyContext : public BaseContext {
public:
    KeyContext(Builder& builder);

    KeyContext key(const std::string& key) = delete;

    BaseContext end_dict() = delete;
    BaseContext end_array() = delete;

    DictionaryContext value(const Node::Value& value);
};

class DictionaryContext : public BaseContext {
public:
    DictionaryContext(Builder& builder);

    DictionaryContext start_dict() = delete;

    ArrayContext start_array() = delete;
    Builder& end_array() = delete;

    Builder& value(const Node::Value& value) = delete;
};

class ArrayContext : public BaseContext {
public:
    ArrayContext(Builder& builder);

    KeyContext key(const std::string& key) = delete;

    Builder& end_dict() = delete;

    ArrayContext value(const Node::Value& value);
};

}//end namespace builder
}//end namespace json
}//end namespace detail
}//end namespace transport_catalogue
-----------------------------------------------------------------------
json_reader.cpp
-----------------------------------------------------------------------
#include "json_reader.h"

namespace transport_catalogue {
namespace detail {
namespace json {
    
JSONReader::JSONReader(Document doc) : document_(std::move(doc)) {}
JSONReader::JSONReader(std::istream& input) : document_(json::load(input)) {}    
    
Stop JSONReader::parse_node_stop(Node& node) {
    Stop stop;
    Dict stop_node;
    
    if (node.is_dict()) {
        stop_node = node.as_dict();
        stop.name = stop_node.at("name").as_string();
        stop.latitude = stop_node.at("latitude").as_double();
        stop.longitude = stop_node.at("longitude").as_double();
    }
    
    return stop;
}
    
std::vector<Distance> JSONReader::parse_node_distances(Node& node, TransportCatalogue& catalogue) {
    std::vector<Distance> distances;
    Dict stop_node;
    Dict stop_road_map;
    std::string begin_name;
    std::string last_name;
    int distance;
    
    if (node.is_dict()) {
        stop_node = node.as_dict();
        begin_name = stop_node.at("name").as_string();
        
        try {
            stop_road_map = stop_node.at("road_distances").as_dict();
            
            for (auto [key, value] : stop_road_map) {
                last_name = key;
                distance = value.as_int();
                distances.push_back({catalogue.get_stop(begin_name), 
                                     catalogue.get_stop(last_name), distance});
            } 
            
        } catch(...) {
            std::cout << "invalide road" << std::endl;
        }        
    }
    
    return distances;
}

Bus JSONReader::parse_node_bus(Node& node, TransportCatalogue& catalogue) {
    Bus bus;
    Dict bus_node;
    Array bus_stops;
    
    if (node.is_dict()) {
        bus_node = node.as_dict();
        bus.name = bus_node.at("name").as_string();
        bus.is_roundtrip = bus_node.at("is_roundtrip").as_bool();

        try {
            bus_stops = bus_node.at("stops").as_array();
            
            for (Node stop : bus_stops) {
                bus.stops.push_back(catalogue.get_stop(stop.as_string()));
            }

            if (!bus.is_roundtrip) {
                size_t size = bus.stops.size() - 1;
                
                for (size_t i = size; i > 0; i--) {
                    bus.stops.push_back(bus.stops[i-1]);
                }
                
            } 
            
        } catch(...) {
            std::cout << "base_requests: bus: stops is empty" << std::endl;
        }        
    }
    
    return bus;
}
    
void JSONReader::parse_node_base(const Node& root, TransportCatalogue& catalogue){
    Array base_requests;
    Dict req_map;
    Node req_node;
    
    std::vector<Node> buses;
    std::vector<Node> stops;
    
    if (root.is_array()) {
        base_requests = root.as_array();
        
        for (Node& node : base_requests) {            
            if (node.is_dict()) {
                req_map = node.as_dict();
                
                try {
                    req_node = req_map.at("type");
                    
                    if (req_node.is_string()) {
                        
                        if (req_node.as_string() == "Bus") {
                            buses.push_back(req_map);
                        } else if (req_node.as_string() == "Stop") {
                            stops.push_back(req_map);
                        } else {
                            std::cout << "base_requests are invalid";
                        }
                    }
                    
                } catch(...) {
                    std::cout << "base_requests does not have type value";
                }                
            }
        }   
        
        for (auto stop : stops) {
            catalogue.add_stop(parse_node_stop(stop));
        }
        
        for (auto stop : stops) {
            catalogue.add_distance(parse_node_distances(stop, catalogue));
        }
        
        for (auto bus : buses) {
            catalogue.add_bus(parse_node_bus(bus, catalogue));
        }
        
    } else {
        std::cout << "base_requests is not an array";
    }  
}

void JSONReader::parse_node_stat(const Node& node, std::vector<StatRequest>& stat_request){
    Array stat_requests;
    Dict req_map;
    StatRequest req;
    
    if (node.is_array()) {
        stat_requests = node.as_array();
        
        for (Node req_node : stat_requests) {
            
            if (req_node.is_dict()) {
                req_map = req_node.as_dict();
                req.id = req_map.at("id").as_int();
                req.type = req_map.at("type").as_string();               

                if ((req.type == "Bus") || (req.type == "Stop")) {
                    req.name = req_map.at("name").as_string();
                    req.from ="";
                    req.to = "";
                    
                } else {
                    req.name = "";
                    if (req.type == "Route") {
                        req.from = req_map.at("from").as_string();
                        req.to = req_map.at("to").as_string();
                        
                    } else {
                        req.from ="";
                        req.to = "";
                    }
                }
                
                stat_request.push_back(req);                
            }   
        } 
        
    } else {
        std::cout << "stat_requests is not array";
    }
}
  
void JSONReader::parse_node_render(const Node& node, map_renderer::RenderSettings& rend_set){
    Dict rend_map;
    Array bus_lab_offset;
    Array stop_lab_offset;
    Array arr_color;
    Array arr_palette;
    uint8_t red_;
    uint8_t green_;
    uint8_t blue_;
    double opacity_;

    if (node.is_dict()) {
        rend_map = node.as_dict();
        
        try {
            rend_set.width_ = rend_map.at("width").as_double();
            rend_set.height_ = rend_map.at("height").as_double();
            rend_set.padding_ = rend_map.at("padding").as_double();
            rend_set.line_width_ = rend_map.at("line_width").as_double();
            rend_set.stop_radius_ = rend_map.at("stop_radius").as_double();
            
            rend_set.bus_label_font_size_ = rend_map.at("bus_label_font_size").as_int();
            
            if (rend_map.at("bus_label_offset").is_array()) {
                bus_lab_offset = rend_map.at("bus_label_offset").as_array();
                rend_set.bus_label_offset_ = std::make_pair(bus_lab_offset[0].as_double(),
                                                            bus_lab_offset[1].as_double());
            }
            
            rend_set.stop_label_font_size_ = rend_map.at("stop_label_font_size").as_int();

            if (rend_map.at("stop_label_offset").is_array()) {
                stop_lab_offset = rend_map.at("stop_label_offset").as_array();
                rend_set.stop_label_offset_ = std::make_pair(stop_lab_offset[0].as_double(),
                                                             stop_lab_offset[1].as_double());
            }
            
            if (rend_map.at("underlayer_color").is_string()) {
                rend_set.underlayer_color_ = svg::Color(rend_map.at("underlayer_color").as_string());
            } else if (rend_map.at("underlayer_color").is_array()) {
                arr_color = rend_map.at("underlayer_color").as_array();
                red_ = arr_color[0].as_int();
                green_ = arr_color[1].as_int();
                blue_ = arr_color[2].as_int();

                if(arr_color.size() == 4){
                    opacity_ = arr_color[3].as_double();
                    rend_set.underlayer_color_ = svg::Color(svg::Rgba(red_, 
                                                                      green_, 
                                                                      blue_, 
                                                                      opacity_));
                } else if (arr_color.size() == 3) {
                    rend_set.underlayer_color_ = svg::Color(svg::Rgb(red_, 
                                                                     green_, 
                                                                     blue_));
                }
                
            }

            rend_set.underlayer_width_ = rend_map.at("underlayer_width").as_double();

            if (rend_map.at("color_palette").is_array()) {
                arr_palette = rend_map.at("color_palette").as_array();
                
                for (Node color_palette : arr_palette) {
                    
                    if (color_palette.is_string()) {
                        rend_set.color_palette_.push_back(svg::Color(color_palette.as_string()));
                    } else if (color_palette.is_array()) {
                        arr_color = color_palette.as_array();
                        red_ = arr_color[0].as_int();
                        green_ = arr_color[1].as_int();
                        blue_ = arr_color[2].as_int();

                        if (arr_color.size() == 4) {
                            opacity_ = arr_color[3].as_double();
                            rend_set.color_palette_.push_back(svg::Color(svg::Rgba(red_, 
                                                                                   green_, 
                                                                                   blue_, 
                                                                                   opacity_)));
                        } else if (arr_color.size() == 3) {
                            rend_set.color_palette_.push_back(svg::Color(svg::Rgb(red_, 
                                                                                  green_, 
                                                                                  blue_)));
                        }
                    }
                }
            }            
        } catch(...) {
            std::cout << "unable to parsse init settings";
        }
        
    } else {
        std::cout << "render_settings is not map";
    }
}
    
void JSONReader::parse_node_routing(const Node& node, router::RoutingSettings& route_set) {
    Dict route;
    
    if (node.is_dict()) {
        route = node.as_dict();

        try {

            route_set.bus_wait_time = route.at("bus_wait_time").as_double();
            route_set.bus_velocity = route.at("bus_velocity").as_double();
            
        } catch(...) {
            std::cout << "unable to parse routing settings";
        }
        
    } else {
        std::cout << "routing settings is not map";
    }

}
    
void JSONReader::parse_node_serialization(const Node& node, serialization::SerializationSettings& serialization_set) {
    
    Dict serialization;

    if (node.is_dict()) {
        serialization = node.as_dict();

        try {
            serialization_set.file_name = serialization.at("file").as_string();

        } catch(...) {
            std::cout << "unable to parse serialization settings";
        }

    } else {
        std::cout << "serialization settings is not map";
    }
}
    
void JSONReader::parse_node_make_base(TransportCatalogue& catalogue, 
                                      map_renderer::RenderSettings& render_settings, 
                                      router::RoutingSettings& routing_settings,
                                      serialization::SerializationSettings& serialization_settings) { 
    Dict root_dictionary;
    
    if (document_.get_root().is_dict()) {
        root_dictionary = document_.get_root().as_dict();
        
        try {          
            parse_node_base(root_dictionary.at("base_requests"), catalogue);
            
        } catch(...) {}   
        
        try {
            parse_node_render(root_dictionary.at("render_settings"), render_settings);
            
        } catch(...) {}
        
        try {
            parse_node_routing(root_dictionary.at("routing_settings"), routing_settings);
            
        } catch(...) {}
        
        try {
            parse_node_serialization(root_dictionary.at("serialization_settings"), serialization_settings);
            
        } catch(...) {}
        
    } else {
        std::cout << "root is not map";
    }
}
    
void JSONReader::parse_node_process_requests(std::vector<StatRequest>& stat_request,
                                             serialization::SerializationSettings& serialization_settings) { 
    Dict root_dictionary;

    if (document_.get_root().is_dict()) {
        root_dictionary = document_.get_root().as_dict();

        try {
            parse_node_stat(root_dictionary.at("stat_requests"), stat_request);
            
        } catch(...) {}

        try {
            parse_node_serialization(root_dictionary.at("serialization_settings"), serialization_settings);
            
        } catch(...) {}

    } else {
        std::cout << "root is not map";
    }
}
    
}//end namespace json
}//end namespace detail
}//end namespace transport_catalogue
-----------------------------------------------------------------------
json_reader.h
-----------------------------------------------------------------------
#pragma once
#include "json.h"
#include "serialization.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

namespace transport_catalogue {
namespace detail {
namespace json {
    
class JSONReader{
public:
    JSONReader() = default;    
    JSONReader(Document doc);
    JSONReader(std::istream& input);
    
    void parse_node_base(const Node& root, TransportCatalogue& catalogue);
    void parse_node_stat(const Node& root, std::vector<StatRequest>& stat_request);
    void parse_node_render(const Node& node, map_renderer::RenderSettings& render_settings);
    void parse_node_routing(const Node& node, router::RoutingSettings& route_set);
    void parse_node_serialization(const Node& node, serialization::SerializationSettings& serialization_set);
    
    void parse_node_make_base(TransportCatalogue& catalogue, 
                              map_renderer::RenderSettings& render_settings, 
                              router::RoutingSettings& routing_settings,
                              serialization::SerializationSettings& serialization_settings);
    
    void parse_node_process_requests(std::vector<StatRequest>& stat_request,
                                     serialization::SerializationSettings& serialization_settings);
    
    Stop parse_node_stop(Node& node);
    Bus parse_node_bus(Node& node, TransportCatalogue& catalogue);
    std::vector<Distance> parse_node_distances(Node& node, TransportCatalogue& catalogue);
    
    const Document& get_document() const;
    
private:
    Document document_;
};
    
}//end namespace json
}//end namespace detail
}//end namespace transport_catalogue
-----------------------------------------------------------------------
json.cpp
-----------------------------------------------------------------------
#include "json.h"

using namespace std;

namespace transport_catalogue {
namespace detail {
namespace json {
namespace {
    
Node load_node(istream& input);
 
std::string load_literal(std::istream& input) {
    std::string str;
    
    while (std::isalpha(input.peek())) {
        str.push_back(static_cast<char>(input.get()));
    }
    return str;
}
 
Node load_array(std::istream& input) {
    std::vector<Node> array;
    
    for (char ch; input >> ch && ch != ']';) {
        if (ch != ',') {
            input.putback(ch);
        }
        
        array.push_back(load_node(input));
    }

    if (!input) {
        throw ParsingError("unable to parse array"s);
    }

    return Node(array);
}
 
Node load_null(std::istream& input) {
    if (auto literal = load_literal(input); literal == "null"sv) {
        return Node(nullptr);
    } else {
        throw ParsingError("unable to parse '"s + literal + "' as null"s);
    }
}
 
Node load_bool(std::istream& input) {
    const auto str = load_literal(input);

    if (str == "true"sv) {
        return Node(true);
    } else if (str == "false"sv) {
        return Node(false);
    } else {
        throw ParsingError("unable to parse '"s + str + "' as bool"s);
    }
}
 
Node load_number(std::istream& input) {
    std::string number;

    auto read_char = [&number, &input] {
        number += static_cast<char>(input.get());
        
        if (!input) {
            throw ParsingError("unable to read number"s);
        }
    };

    auto read_digits = [&input, read_char] {
        
        if (!std::isdigit(input.peek())) {
            throw ParsingError("digit expected"s);
        } else {
            while (std::isdigit(input.peek())) {
                read_char();
            }   
        }
    };

    if (input.peek() == '-') {
        read_char();
    }

    if (input.peek() == '0') {
        read_char();
    } else {
        read_digits();
    }

    bool is_int = true;
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }

        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            try {
                return Node(std::stoi(number));
                
            } catch (...) {}
        }
        return Node(std::stod(number));
        
    } catch (...) {
        throw ParsingError("unable to convert "s + number + " to number"s);
    }
}
 
Node load_string(std::istream& input) {
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string str;
    
    while (true) {
        if (it == end) {
            throw ParsingError("unable to parse string");
        }
        
        const char ch = *it;
        if (ch == '"') {
            ++it;
            break;
            
        } else if (ch == '\\') {
            ++it;
            if (it == end) {
                throw ParsingError("unable to parse string");
            }
            
            const char esc_ch = *(it);
            switch (esc_ch) {
                case 'n':
                    str.push_back('\n');
                    break;
                case 't':
                    str.push_back('\t');
                    break;
                case 'r':
                    str.push_back('\r');
                    break;
                case '"':
                    str.push_back('"');
                    break;
                case '\\':
                    str.push_back('\\');
                    break;
                default:
                    throw ParsingError("invalid esc \\"s + esc_ch);
            }
            
        } else if (ch == '\n' || ch == '\r') {
            throw ParsingError("invalid line end"s);
        } else {
            str.push_back(ch);
        }
        
        ++it;
    }
    
    return Node(str);
}
    
Node load_dictionary(std::istream& input) {
    Dict dictionary;

    for (char ch; input >> ch && ch != '}';) {
        
        if (ch == '"') {
            std::string key = load_string(input).as_string();

            if (input >> ch && ch == ':') {
                
                if (dictionary.find(key) != dictionary.end()) {
                    throw ParsingError("duplicate key '"s + key + "'found");
                }

                dictionary.emplace(std::move(key), load_node(input));
                
            } else {
                throw ParsingError(": expected. but '"s + ch + "' found"s);
            }
            
        } else if (ch != ',') {
            throw ParsingError("',' expected. but '"s + ch + "' found"s);
        }
    }

    if (!input) {
        throw ParsingError("unable to parse dictionary"s);
    } else {
        return Node(dictionary);
    }
    
}
  
Node load_node(std::istream& input) {
    char ch;
    
    if (!(input >> ch)) {
        throw ParsingError(""s);
    } else {
        switch (ch) {
        case '[':
            return load_array(input);
        case '{':
            return load_dictionary(input);
        case '"':
            return load_string(input);
        case 't': case 'f':
            input.putback(ch);
            return load_bool(input);
        case 'n':
            input.putback(ch);
            return load_null(input);
        default:
            input.putback(ch);
            return load_number(input);
        }
    }
} 
    
}//end namespace
   
Node::Node(Array array) : value_(std::move(array)) {}    
Node::Node(std::nullptr_t) : Node() {}
Node::Node(bool value) : value_(value) {}
Node::Node(Dict dict) : value_(std::move(dict)) {}
Node::Node(int value) : value_(value) {}
Node::Node(string value) : value_(std::move(value)) {}
Node::Node(double value) : value_(value) {}
    
const Array& Node::as_array() const {
    using namespace std::literals;
    
    if (!is_array()) {
        throw std::logic_error("value is not an array"s);
    } else {
        return std::get<Array>(value_); 
    }
}

const Dict& Node::as_dict() const {
    using namespace std::literals;
    
    if (!is_dict()) {
        throw std::logic_error("value is not a dictionary"s);
    } else {
        return std::get<Dict>(value_);  
    }
}

const std::string& Node::as_string() const {
    using namespace std::literals;
    
    if (!is_string()) {
        throw std::logic_error("value is not a string"s);
    } else {
        return std::get<std::string>(value_);        
    }
}
    
int Node::as_int() const {
    using namespace std::literals; 
    
    if (!is_int()) {
        throw std::logic_error("value is not an int"s);
    } else {
        return std::get<int>(value_);    
    }
}

double Node::as_double() const {
    using namespace std::literals;
    
    if (!is_double()) {
        throw std::logic_error("value is not a double"s);
    } else if (is_real_double()) {
        return std::get<double>(value_);
    } else {
        return as_int();
    }   
}

bool Node::as_bool() const {
    using namespace std::literals;
    
    if (!is_bool()) {
        throw std::logic_error("value is not a bool"s);
    } else {
        return std::get<bool>(value_);   
    }
}
    
bool Node::is_null() const {return std::holds_alternative<std::nullptr_t>(value_);}
bool Node::is_int() const {return std::holds_alternative<int>(value_);}
bool Node::is_double() const {return is_real_double() || is_int();}  
bool Node::is_real_double() const {return std::holds_alternative<double>(value_);}
bool Node::is_bool() const {return std::holds_alternative<bool>(value_);}
bool Node::is_string() const {return std::holds_alternative<std::string>(value_);}
bool Node::is_array() const {return std::holds_alternative<Array>(value_);}
bool Node::is_dict() const {return std::holds_alternative<Dict>(value_);}

const Node::Value& Node::get_value() const {return value_;}
   
Document::Document(Node root) : root_(std::move(root)) {}
const Node& Document::get_root() const {return root_;}
    
Document load(istream& input) {return Document(load_node(input));}

struct PrintContext {
    std::ostream& out;
    int indent_step = 4;
    int indent = 0;

    void print_indent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    [[nodiscard]] PrintContext indented() const {
        return {out, 
                indent_step, 
                indent_step + indent};
    }
};
 
void print_node(const Node& node, const PrintContext& context);
 
void print_string(const std::string& value, std::ostream& out) {
    out.put('"');
    
    for (const char ch : value) {
        switch (ch) {
            case '\r':
                out << R"(\r)";
                break;
            case '\n':
                out << R"(\n)";
                break;
            case '"':
                out << R"(\")";
                break;
            case '\\':
                out << R"(\\)";
                break;
            default:
                out.put(ch);
                break;
        }
    }
    
    out.put('"');
}

template <typename Value>
void print_value(const Value& value, const PrintContext& context) {
    context.out << value;
}    

template <>
void print_value<std::string>(const std::string& value, const PrintContext& context) {
    print_string(value, context.out);
}
 
void print_value(const std::nullptr_t&, const PrintContext& context) {
    context.out << "null"s;
}
 
void print_value(bool value, const PrintContext& context) {
    context.out << std::boolalpha << value;
}
 
[[maybe_unused]] void print_value(Array nodes, const PrintContext& context) {
    std::ostream& out = context.out;
    out << "[\n"sv;
    bool first = true;
    auto inner_context = context.indented();
    
    for (const Node& node : nodes) {
        if (first) {
            first = false;
        } else {
            out << ",\n"sv;
        }

        inner_context.print_indent();
        print_node(node, inner_context);
    }

    out.put('\n');
    context.print_indent();
    out.put(']');
}
 
[[maybe_unused]] void print_value(Dict nodes, const PrintContext& context) {
    std::ostream& out = context.out;
    out << "{\n"sv;
    bool first = true;
    auto inner_context = context.indented();
    
    for (const auto& [key, node] : nodes) {
        if (first) {
            first = false;
        } else {
            out << ",\n"sv;
        }

        inner_context.print_indent();
        print_string(key, context.out);
        out << ": "sv;
        print_node(node, inner_context);
    }

    out.put('\n');
    context.print_indent();
    out.put('}');
}
 
void print_node(const Node& node, const PrintContext& context) {
    std::visit([&context](const auto& value) {
            print_value(value, context);
        }, node.get_value());
}
    
void print(const Document& document, std::ostream& output) {
    print_node(document.get_root(), PrintContext{output});
}

}//end namespace json
}//end namespace detail
}//end namespace transport_catalogue
-----------------------------------------------------------------------
json.h
-----------------------------------------------------------------------
#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace transport_catalogue {
namespace detail {
namespace json {

class Node;

using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node final : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
public:
    
    using variant::variant;
    using Value = variant;

    Node() = default;
    Node(bool value);
    Node(Array array);
    Node(Dict dict);
    Node(int value);
    Node(std::string value);
    Node(std::nullptr_t);
    Node(double value);
    
    const Array& as_array() const;
    const Dict& as_dict() const;
    int as_int() const;
    double as_double() const;
    bool as_bool() const;
    const std::string& as_string() const;

    bool is_null() const;
    bool is_int() const;
    bool is_double() const;
    bool is_real_double() const;
    bool is_bool() const;
    bool is_string() const;
    bool is_array() const;
    bool is_dict() const;

    const Value& get_value() const;
        
private:
    Value value_;
};

inline bool operator==(const Node& lhs, const Node& rhs) { 
    return lhs.get_value() == rhs.get_value();
}  
inline bool operator!=(const Node& lhs, const Node& rhs) {
    return !(lhs == rhs);
} 
    
class Document {
public:
    Document() = default;
    explicit Document(Node root);
    const Node& get_root() const;

private:
    Node root_;
};

inline bool operator==(const Document& lhs, const Document& rhs) {
    return lhs.get_root() == rhs.get_root();
}
inline bool operator!=(const Document& lhs, const Document& rhs) {
    return !(lhs == rhs);
}

Document load(std::istream& input);
void print(const Document& document, std::ostream& output);

}//end namespace json
}//end namespace detail
}//end namespace transport_catalogue
-----------------------------------------------------------------------
map_renderer.cpp
-----------------------------------------------------------------------
#include "map_renderer.h"

namespace map_renderer {

bool SphereProjector::is_zero(double value) {
    return std::abs(value) < EPSILON;
}
    
MapRenderer::MapRenderer(RenderSettings& render_settings) : render_settings_(render_settings) {}

svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
    return {(coords.longitude - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.latitude) * zoom_coeff_ + padding_};
}
    
SphereProjector MapRenderer::get_sphere_projector(const std::vector<geo::Coordinates>& points) const {
        return SphereProjector(points.begin(),
                               points.end(),
                               render_settings_.width_,
                               render_settings_.height_,
                               render_settings_.padding_);
}
  
void MapRenderer::init_sphere_projector(std::vector<geo::Coordinates> points){
    sphere_projector = SphereProjector(points.begin(),
                               points.end(),
                               render_settings_.width_,
                               render_settings_.height_,
                               render_settings_.padding_);
}
    
RenderSettings MapRenderer::get_render_settings() const{
    return render_settings_;
}
  
int MapRenderer::get_palette_size() const{
    return render_settings_.color_palette_.size();
}
    
svg::Color MapRenderer::get_color(int line_number) const {
    return render_settings_.color_palette_[line_number];
}

void MapRenderer::set_line_properties(svg::Polyline& polyline, 
                                      [[maybe_unused]] int line_number) const {
    using namespace std::literals;
    
    polyline.set_stroke_color(get_color(line_number));   
    polyline.set_fill_color("none"s);
    polyline.set_stroke_width(render_settings_.line_width_);
    polyline.set_stroke_linecap(svg::StrokeLineCap::ROUND);
    polyline.set_stroke_linejoin(svg::StrokeLineJoin::ROUND);   
} 
    
void MapRenderer::set_route_text_common_properties(svg::Text& text, 
                                                   const std::string& name, 
                                                   svg::Point position) const {
    using namespace std::literals;

    text.set_position(position);
    text.set_offset({render_settings_.bus_label_offset_.first,
                     render_settings_.bus_label_offset_.second});
    text.set_font_size(render_settings_.bus_label_font_size_);
    text.set_font_family("Verdana");
    text.set_font_weight("bold");
    text.set_data(name);   
} 
    
void MapRenderer::set_route_text_additional_properties(svg::Text& text, 
                                                       const std::string& name, 
                                                       svg::Point position) const {
    set_route_text_common_properties(text, 
                                     name, 
                                     position);
    
    text.set_fill_color(render_settings_.underlayer_color_);
    text.set_stroke_color(render_settings_.underlayer_color_);
    text.set_stroke_width(render_settings_.underlayer_width_);
    text.set_stroke_linejoin(svg::StrokeLineJoin::ROUND);
    text.set_stroke_linecap(svg::StrokeLineCap::ROUND);
}

void MapRenderer::set_route_text_color_properties(svg::Text& text, 
                                                  const std::string& name, 
                                                  int palette, 
                                                  svg::Point position) const {
    set_route_text_common_properties(text, 
                                     name, 
                                     position);
    
    text.set_fill_color(get_color(palette));
}
    
void MapRenderer::set_stops_circles_properties(svg::Circle& circle, 
                                               svg::Point position) const {
    using namespace std::literals;
    
    circle.set_center(position);
    circle.set_radius(render_settings_.stop_radius_);
    circle.set_fill_color("white");
} 
    
void MapRenderer::set_stops_text_common_properties(svg::Text& text, 
                                                   const std::string& name, 
                                                   svg::Point position) const {
    using namespace std::literals;

    text.set_position(position);
    text.set_offset({render_settings_.stop_label_offset_.first,
                     render_settings_.stop_label_offset_.second});
    text.set_font_size(render_settings_.stop_label_font_size_);
    text.set_font_family("Verdana");
    text.set_data(name);
}
 
void MapRenderer::set_stops_text_additional_properties(svg::Text& text, 
                                                       const std::string& name, 
                                                       svg::Point position) const {
    using namespace std::literals;
    set_stops_text_common_properties(text, 
                                     name, 
                                     position);

    text.set_fill_color(render_settings_.underlayer_color_);
    text.set_stroke_color(render_settings_.underlayer_color_);
    text.set_stroke_width(render_settings_.underlayer_width_);
    text.set_stroke_linejoin(svg::StrokeLineJoin::ROUND);
    text.set_stroke_linecap(svg::StrokeLineCap::ROUND);
}
 
void MapRenderer::set_stops_text_color_properties(svg::Text& text, 
                                                  const std::string& name, 
                                                  svg::Point position) const {
    using namespace std::literals;

    set_stops_text_common_properties(text, 
                                     name, 
                                     position);
    text.set_fill_color("black");
}
    
void MapRenderer::add_line(std::vector<std::pair<Bus*, int>>& buses_palette) {    
    std::vector<geo::Coordinates> stops_geo_coords;
    
    for (auto [bus, palette] : buses_palette) { 
        
        for (Stop* stop : bus->stops) {
            geo::Coordinates coordinates;
            coordinates.latitude = stop->latitude;
            coordinates.longitude = stop->longitude;
            
            stops_geo_coords.push_back(coordinates); 
        }
        
        svg::Polyline bus_line;
        bool bus_empty = true;   
        
        for (auto& coord : stops_geo_coords) {
            bus_empty = false;
            bus_line.add_point(sphere_projector(coord));
        } 
        
        if (!bus_empty) {
            set_line_properties(bus_line, 
                                palette);
            map_svg.add(bus_line);   
        }
        
        stops_geo_coords.clear();
    }
}
    
void MapRenderer::add_buses_name(std::vector<std::pair<Bus*, int>>& buses_palette){    
    std::vector<geo::Coordinates> stops_geo_coords;
    bool bus_empty = true; 
    
    for (auto [bus, palette] : buses_palette) {  
        
        for (Stop* stop : bus->stops) {
            geo::Coordinates coordinates;
            coordinates.latitude = stop->latitude;
            coordinates.longitude = stop->longitude;
            
            stops_geo_coords.push_back(coordinates); 
            
            if(bus_empty) bus_empty = false;
        }
        
        svg::Text route_name_roundtrip;
        svg::Text route_title_roundtrip;
        svg::Text route_name_notroundtrip;
        svg::Text route_title_notroundtrip; 
        
        if (!bus_empty) {
            
            if (bus->is_roundtrip) {
                set_route_text_additional_properties(route_name_roundtrip,
                                                     std::string(bus->name),
                                                     sphere_projector(stops_geo_coords[0]));
                map_svg.add(route_name_roundtrip);
                
                set_route_text_color_properties(route_title_roundtrip,
                                                std::string(bus->name),
                                                palette,
                                                sphere_projector(stops_geo_coords[0]));
                map_svg.add(route_title_roundtrip);
                
            } else {
                set_route_text_additional_properties(route_name_roundtrip,
                                                     std::string(bus->name),
                                                     sphere_projector(stops_geo_coords[0]));
                map_svg.add(route_name_roundtrip);
                
                set_route_text_color_properties(route_title_roundtrip,
                                                std::string(bus->name),
                                                palette,
                                                sphere_projector(stops_geo_coords[0]));
                map_svg.add(route_title_roundtrip);
                
                if (stops_geo_coords[0] != stops_geo_coords[stops_geo_coords.size()/2]) {
                    set_route_text_additional_properties(route_name_notroundtrip,
                                                         std::string(bus->name),
                                                         sphere_projector(stops_geo_coords[stops_geo_coords.size()/2]));
                    map_svg.add(route_name_notroundtrip);
                    
                    set_route_text_color_properties(route_title_notroundtrip,
                                                    std::string(bus->name),
                                                    palette,
                                                    sphere_projector(stops_geo_coords[stops_geo_coords.size()/2]));
                    map_svg.add(route_title_notroundtrip);
                }
            } 
        }
        
        bus_empty = false;
        stops_geo_coords.clear();
    }
}
    
void MapRenderer::add_stops_circle(std::vector<Stop*>& stops){
    std::vector<geo::Coordinates> stops_geo_coords;      
    svg::Circle icon;
    
    for (Stop* stop_info : stops) { 
        
        if (stop_info) {
            geo::Coordinates coordinates;
            coordinates.latitude = stop_info->latitude;
            coordinates.longitude = stop_info->longitude; 

            set_stops_circles_properties(icon, 
                                         sphere_projector(coordinates));
            map_svg.add(icon);  
        }
    }
}
  
void MapRenderer::add_stops_name(std::vector<Stop*>& stops){    
    std::vector<geo::Coordinates> stops_geo_coords;    
    
    svg::Text svg_stop_name;
    svg::Text svg_stop_name_title;
    
    for (Stop* stop_info : stops) {
        
        if (stop_info) {
            geo::Coordinates coordinates;
            coordinates.latitude = stop_info->latitude;
            coordinates.longitude = stop_info->longitude;            
            
            set_stops_text_additional_properties(svg_stop_name, 
                                                 stop_info->name, 
                                                 sphere_projector(coordinates));
            map_svg.add(svg_stop_name);
            
            set_stops_text_color_properties(svg_stop_name_title, 
                                            stop_info->name, 
                                            sphere_projector(coordinates));
            map_svg.add(svg_stop_name_title); 
        }
    }
}
  
void MapRenderer::get_stream_map(std::ostream& stream_) { 
    map_svg.render(stream_);
}
    
}//end namespace map_renderer
-----------------------------------------------------------------------
map_renderer.h
-----------------------------------------------------------------------
#pragma once
#include <iostream>
#include <optional>
#include <algorithm>
#include <cstdlib>
 
#include "domain.h"
#include "geo.h"
#include "svg.h"

using namespace domain;

namespace map_renderer {
    
inline const double EPSILON = 1e-6;

class SphereProjector {
public:
    SphereProjector() = default;
    
    template <typename InputIt>
    SphereProjector(InputIt points_begin, 
                    InputIt points_end,
                    double max_width, 
                    double max_height, 
                    double padding);

    svg::Point operator()(geo::Coordinates coords) const;

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
    
    bool is_zero(double value);
};
    
struct RenderSettings {
    double width_;
    double height_;
    double padding_;
    double line_width_;
    double stop_radius_;
    int bus_label_font_size_;
    std::pair<double, double> bus_label_offset_;
    int stop_label_font_size_;
    std::pair<double, double> stop_label_offset_;
    svg::Color underlayer_color_;
    double underlayer_width_;
    std::vector<svg::Color> color_palette_;
};
    
class MapRenderer {

public:
    MapRenderer(RenderSettings& render_settings);   
    
    SphereProjector get_sphere_projector(const std::vector<geo::Coordinates>& points) const;
    void init_sphere_projector(std::vector<geo::Coordinates> points);
    
    RenderSettings get_render_settings() const;
    int get_palette_size() const;
    svg::Color get_color(int line_number) const;
        
    void set_line_properties(svg::Polyline& polyline, int line_number) const; 
    
    void set_route_text_common_properties(svg::Text &text, const std::string& name, svg::Point position) const;
    void set_route_text_additional_properties(svg::Text& text, const std::string& name, svg::Point position) const;
    void set_route_text_color_properties(svg::Text& text, const std::string& name, int palette, svg::Point position) const;
    
    void set_stops_circles_properties(svg::Circle& circle, svg::Point position) const;
    
    void set_stops_text_common_properties(svg::Text& text, const std::string& name, svg::Point position) const;
    void set_stops_text_additional_properties(svg::Text& text, const std::string& name, svg::Point position) const;
    void set_stops_text_color_properties(svg::Text& text, const std::string& name, svg::Point position) const;
    
    void add_line(std::vector<std::pair<Bus*, int>>& buses_palette);
    void add_buses_name(std::vector<std::pair<Bus*, int>>& buses_palette);
    void add_stops_circle(std::vector<Stop*>& stops_name);
    void add_stops_name(std::vector<Stop*>& stops_name);    
    
    void get_stream_map(std::ostream& stream_);
    
private:
    SphereProjector sphere_projector;
    RenderSettings& render_settings_;
    svg::Document map_svg;
};

template <typename InputIt>
    SphereProjector::SphereProjector(InputIt points_begin, 
                                     InputIt points_end,
                                     double max_width, 
                                     double max_height, 
                                     double padding) : padding_(padding) {
        if (points_begin == points_end) {
            return;
        }

        const auto [left_it, 
                    right_it] = std::minmax_element(points_begin, 
                                                    points_end,
            [](auto lhs, auto rhs) { 
                return lhs.longitude < rhs.longitude;
            });
        
        min_lon_ = left_it->longitude;
        const double max_lon = right_it->longitude;

        const auto [bottom_it, 
                    top_it] = std::minmax_element(points_begin, 
                                                  points_end,
            [](auto lhs, auto rhs) { 
                return lhs.latitude < rhs.latitude;
            });
        
        const double min_lat = bottom_it->latitude;
        max_lat_ = top_it->latitude;

        std::optional<double> width_zoom;
        if (!is_zero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) 
                         / (max_lon - min_lon_);
        }

        std::optional<double> height_zoom;
        if (!is_zero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) 
                          / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            zoom_coeff_ = std::min(*width_zoom, 
                                   *height_zoom);     
        } else if (width_zoom) {
            zoom_coeff_ = *width_zoom;
            
        } else if (height_zoom) {
            zoom_coeff_ = *height_zoom;
            
        }
    }
    
}//end namespace map_renderer
-----------------------------------------------------------------------
ranges.h
-----------------------------------------------------------------------
#pragma once

#include <iterator>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace ranges {

template <typename It>
class Range {
public:
    using ValueType = typename std::iterator_traits<It>::value_type;

    Range(It begin, It end) : begin_(begin)
                            , end_(end) {}
                            
    It begin() const {return begin_;}
    It end() const {return end_;}

private:
    It begin_;
    It end_;
};

template <typename C>
auto as_range(const C& container) {return Range{container.begin(), container.end()};}

}//end namespace ranges
-----------------------------------------------------------------------
request_handler.cpp
-----------------------------------------------------------------------
#include "request_handler.h"

namespace request_handler {

struct EdgeInfoGetter {
    
    Node operator()(const StopEdge& edge_info) {
        using namespace std::literals;

        return Builder{}.start_dict()
                        .key("type").value("Wait")
                        .key("stop_name").value(std::string(edge_info.name))
                        .key("time").value(edge_info.time)
                        .end_dict()
                        .build();
    }

    Node operator()(const BusEdge& edge_info) {
        using namespace std::literals;
    
        return Builder{}.start_dict()
                        .key("type").value("Bus")
                        .key("bus").value(std::string(edge_info.bus_name))
                        .key("span_count").value(static_cast<int>(edge_info.span_count))
                        .key("time").value(edge_info.time)
                        .end_dict()
                        .build();
    }
};

Node RequestHandler::execute_make_node_stop(int id_request, const StopQueryResult& stop_info) {
    Node result;
    Array buses;
    Builder builder;

    std::string str_not_found = "not found";

    if (stop_info.not_found) {
        builder.start_dict()
               .key("request_id").value(id_request)
               .key("error_message").value(str_not_found)
               .end_dict();
        
        result = builder.build();
        
    } else {
        builder.start_dict()
               .key("request_id").value(id_request)
               .key("buses").start_array();

        for (std::string bus_name : stop_info.buses_name) {
            builder.value(bus_name);
        }

        builder.end_array().end_dict();
        
        result = builder.build();
    }

    return result;
}

Node RequestHandler::execute_make_node_bus(int id_request, const BusQueryResult& bus_info) {
    Node result;
    std::string str_not_found = "not found";

    if (bus_info.not_found) {
        result = Builder{}.start_dict()
                          .key("request_id").value(id_request)
                          .key("error_message").value(str_not_found)
                          .end_dict()
                          .build();
    } else {
        result = Builder{}.start_dict()
                          .key("request_id").value(id_request)
                          .key("curvature").value(bus_info.curvature)
                          .key("route_length").value(bus_info.route_length)
                          .key("stop_count").value(bus_info.stops_on_route)
                          .key("unique_stop_count").value(bus_info.unique_stops)
                          .end_dict()
                          .build();
    }

    return result;
}

Node RequestHandler::execute_make_node_map(int id_request, 
                                           TransportCatalogue& catalogue_, 
                                           RenderSettings render_settings) {
    Node result;

    std::ostringstream map_stream;
    std::string map_str;

    MapRenderer map_catalogue(render_settings);
    
    map_catalogue.init_sphere_projector(get_stops_coordinates(catalogue_));
    
    execute_render_map(map_catalogue, catalogue_);
    map_catalogue.get_stream_map(map_stream);
    map_str = map_stream.str();

    result = Builder{}.start_dict()
                      .key("request_id").value(id_request)
                      .key("map").value(map_str)
                      .end_dict()
                      .build();

    return result;
}

Node RequestHandler::execute_make_node_route(StatRequest& request, 
                                             TransportCatalogue& catalogue, 
                                             TransportRouter& routing) {
    const auto& route_info = get_route_info(request.from, request.to, catalogue, routing);

    if (!route_info) {
        return Builder{}.start_dict()
                        .key("request_id").value(request.id)
                        .key("error_message").value("not found")
                        .end_dict()
                        .build();
    }

    Array items;
    for (const auto& item : route_info->edges) {
        items.emplace_back(std::visit(EdgeInfoGetter{}, item));
    }

    return Builder{}.start_dict()
                    .key("request_id").value(request.id)
                    .key("total_time").value(route_info->total_time)
                    .key("items").value(items)
                    .end_dict()
                    .build();
}

void RequestHandler::execute_queries(TransportCatalogue& catalogue,
                                     std::vector<StatRequest>& stat_requests,
                                     RenderSettings& render_settings,
                                     RoutingSettings& routing_settings) {

    std::vector<Node> result_request;
    
    for (StatRequest req : stat_requests) {

        if (req.type == "Stop") {
            result_request.push_back(execute_make_node_stop(req.id, stop_query(catalogue, req.name)));
            
        } else if (req.type == "Bus") {
            result_request.push_back(execute_make_node_bus(req.id, bus_query(catalogue, req.name)));
            
        } else if (req.type == "Map") {
            result_request.push_back(execute_make_node_map(req.id, catalogue, render_settings));
        }   
    }

    doc_out = Document{Node(result_request)};
}

void RequestHandler::execute_render_map(MapRenderer& map_catalogue, TransportCatalogue& catalogue) const {
    std::vector<std::pair<Bus*, int>> buses_palette;
    std::vector<Stop*> stops_sort;
    int palette_size = 0;
    int palette_index = 0;

    palette_size = map_catalogue.get_palette_size();
    
    if (palette_size == 0) {
        std::cout << "color palette is empty";
        return;
    }

    auto buses = catalogue.get_busname_to_bus();
    if (buses.size() > 0) {

        for (std::string_view bus_name : get_sort_buses_names(catalogue)) {
            Bus* bus_info = catalogue.get_bus(bus_name);

            if (bus_info) {
                if (bus_info->stops.size() > 0) {
                    buses_palette.push_back(std::make_pair(bus_info, palette_index));
                    palette_index++;

                    if (palette_index == palette_size) {
                        palette_index = 0;
                    }
                }
            }
        }

        if (buses_palette.size() > 0) {
            map_catalogue.add_line(buses_palette);
            map_catalogue.add_buses_name(buses_palette);
        }
    }

    auto stops = catalogue.get_stopname_to_stop();
    if (stops.size() > 0) {
        std::vector<std::string_view> stops_name;

        for (auto& [stop_name, stop] : stops) {

            if (stop->buses.size() > 0) {
                stops_name.push_back(stop_name);
            }
        }

        std::sort(stops_name.begin(), stops_name.end());

        for (std::string_view stop_name : stops_name) {
            Stop* stop = catalogue.get_stop(stop_name);
            if (stop) {
                stops_sort.push_back(stop);
            }
        }

        if (stops_sort.size() > 0) {
            map_catalogue.add_stops_circle(stops_sort);
            map_catalogue.add_stops_name(stops_sort);
        }
    }
}

std::optional<RouteInfo> RequestHandler::get_route_info(std::string_view start, 
                                                        std::string_view end, 
                                                        TransportCatalogue& catalogue, 
                                                        TransportRouter& routing) const {

    return routing.get_route_info(routing.get_router_by_stop(catalogue.get_stop(start))->bus_wait_start, 
                                  routing.get_router_by_stop(catalogue.get_stop(end))->bus_wait_start);
}

std::vector<geo::Coordinates> RequestHandler::get_stops_coordinates(TransportCatalogue& catalogue_) const {

    std::vector<geo::Coordinates> stops_coordinates;
    auto buses = catalogue_.get_busname_to_bus();

    for (auto& [busname, bus] : buses) {
        
        for (auto& stop : bus->stops) {
            geo::Coordinates coordinates;
            coordinates.latitude = stop->latitude;
            coordinates.longitude = stop->longitude;

            stops_coordinates.push_back(coordinates);
        }
    }
    return stops_coordinates;
}

std::vector<std::string_view> RequestHandler::get_sort_buses_names(TransportCatalogue& catalogue_) const {
    std::vector<std::string_view> buses_names;

    auto buses = catalogue_.get_busname_to_bus();
    if (buses.size() > 0) {

        for (auto& [busname, bus] : buses) {
            buses_names.push_back(busname);
        }

        std::sort(buses_names.begin(), buses_names.end());

        return buses_names;

    } else {
        return {};
    }
}

BusQueryResult RequestHandler::bus_query(TransportCatalogue& catalogue, std::string_view bus_name) {
    BusQueryResult bus_info;
    Bus* bus = catalogue.get_bus(bus_name);

    if (bus != nullptr) {
        bus_info.name = bus->name;
        bus_info.not_found = false;
        bus_info.stops_on_route = static_cast<int>(bus->stops.size());
        bus_info.unique_stops = static_cast<int>(catalogue.get_uniq_stops(bus).size());
        bus_info.route_length = static_cast<int>(bus->route_length);
        bus_info.curvature = double(catalogue.get_distance_to_bus(bus)
                                   /catalogue.get_length(bus));
    } else {
        bus_info.name = bus_name;
        bus_info.not_found = true;
    }

    return bus_info;
}

StopQueryResult RequestHandler::stop_query(TransportCatalogue& catalogue, std::string_view stop_name) {
    std::unordered_set<const Bus*> unique_buses;
    StopQueryResult stop_info;
    Stop* stop = catalogue.get_stop(stop_name);

    if (stop != NULL) {

        stop_info.name = stop->name;
        stop_info.not_found = false;
        unique_buses = catalogue.stop_get_uniq_buses(stop);

        if (unique_buses.size() > 0) {
            
            for (const Bus* bus : unique_buses) {
                stop_info.buses_name.push_back(bus->name);
            }

            std::sort(stop_info.buses_name.begin(), stop_info.buses_name.end());
        }
        
    } else {
        stop_info.name = stop_name;
        stop_info.not_found = true;
    }

    return stop_info;
}

const Document& RequestHandler::get_document() {
    return doc_out;
}

}//end namespace request_handler
-----------------------------------------------------------------------
request_handler.h
-----------------------------------------------------------------------
#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "json_builder.h"
#include "transport_router.h"

using namespace transport_catalogue;
using namespace map_renderer;
using namespace transport_catalogue::detail::json;
using namespace transport_catalogue::detail::json::builder;
using namespace transport_catalogue::detail::router;

namespace request_handler {

class RequestHandler {
public:
           
    RequestHandler() = default;
    
    std::optional<RouteInfo> get_route_info(std::string_view start, 
                                            std::string_view end, 
                                            TransportCatalogue& catalogue, 
                                            TransportRouter& routing) const;
    
    std::vector<geo::Coordinates> get_stops_coordinates(TransportCatalogue& catalogue_) const;
    std::vector<std::string_view> get_sort_buses_names(TransportCatalogue& catalogue_) const;
    
    BusQueryResult bus_query(TransportCatalogue& catalogue, std::string_view str);
    StopQueryResult stop_query(TransportCatalogue& catalogue, std::string_view stop_name);
    
    Node execute_make_node_stop(int id_request, const StopQueryResult& query_result);
    Node execute_make_node_bus(int id_request, const BusQueryResult& query_result);
    Node execute_make_node_map(int id_request, TransportCatalogue& catalogue, RenderSettings render_settings);
    Node execute_make_node_route(StatRequest& request, TransportCatalogue& catalogue, TransportRouter& routing);
    
    void execute_queries(TransportCatalogue& catalogue, 
                         std::vector<StatRequest>& stat_requests, 
                         RenderSettings& render_settings,
                         RoutingSettings& route_settings);
    
    void execute_render_map(MapRenderer& map_catalogue, TransportCatalogue& catalogue_) const;
       
    const Document& get_document();

private:
    Document doc_out;
};
    
}//end namespace request_handler
-----------------------------------------------------------------------
router.h
-----------------------------------------------------------------------
#pragma once

#include "graph.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

namespace graph {

template <typename Weight>
class Router {
using Graph = DirectedWeightedGraph<Weight>;

public:
    explicit Router(const Graph& graph);

    struct RouteInfo {
        Weight weight;
        std::vector<EdgeId> edges;
    };
    
    void build() {
		initialize_routes_internal_data(graph_);
		const size_t vertex_count = graph_.get_vertex_count();
        
		for (VertexId vertex_through = 0; vertex_through < vertex_count; ++vertex_through) {
			relax_routes_internal_data_through_vertex(vertex_count, vertex_through);
		}
	}
    
    std::optional<RouteInfo> build_route(VertexId from, VertexId to) const;

private:
    struct RouteInternalData {
        Weight weight;
        std::optional<EdgeId> prev_edge;
    };
    
    using RoutesInternalData = std::vector<std::vector<std::optional<RouteInternalData>>>;

    void initialize_routes_internal_data(const Graph& graph) {
        const size_t vertex_count = graph.get_vertex_count();
        
        for (VertexId vertex = 0; vertex < vertex_count; ++vertex) {
            routes_internal_data_[vertex][vertex] = RouteInternalData{ZERO_WEIGHT, std::nullopt};
            
            for (const EdgeId edge_id : graph.get_incident_edges(vertex)) {
                const auto& edge = graph.get_edge(edge_id);
                
                if (edge.weight < ZERO_WEIGHT) {
                    throw std::domain_error("Edges' weights should be non-negative");
                }
                
                auto& route_internal_data = routes_internal_data_[vertex][edge.to];
                if (!route_internal_data || route_internal_data->weight > edge.weight) {
                    route_internal_data = RouteInternalData{edge.weight, edge_id};
                }
            }
        }
    }

    void relax_route(VertexId vertex_from, VertexId vertex_to, 
                     const RouteInternalData& route_from,
                     const RouteInternalData& route_to) {
                     
        auto& route_relaxing = routes_internal_data_[vertex_from][vertex_to];
        const Weight candidate_weight = route_from.weight + route_to.weight;
        
        if (!route_relaxing || candidate_weight < route_relaxing->weight) {
            route_relaxing = {candidate_weight,
                              route_to.prev_edge ? route_to.prev_edge : route_from.prev_edge};
        }
    }

    void relax_routes_internal_data_through_vertex(size_t vertex_count, VertexId vertex_through) {
    
        for (VertexId vertex_from = 0; vertex_from < vertex_count; ++vertex_from) {
        
            if (const auto& route_from = routes_internal_data_[vertex_from][vertex_through]) {
            
                for (VertexId vertex_to = 0; vertex_to < vertex_count; ++vertex_to) {
                
                    if (const auto& route_to = routes_internal_data_[vertex_through][vertex_to]) {
                        relax_route(vertex_from, vertex_to, *route_from, *route_to);
                    }
                }
            }
        }
    }

    static constexpr Weight ZERO_WEIGHT{};
    const Graph& graph_;
    RoutesInternalData routes_internal_data_;
};

template <typename Weight>
Router<Weight>::Router(const Graph& graph) : graph_(graph)
                                           , routes_internal_data_(graph.get_vertex_count()
                                           , std::vector<std::optional<RouteInternalData>>(graph.get_vertex_count())) {
    initialize_routes_internal_data(graph);

    const size_t vertex_count = graph.get_vertex_count();
    for (VertexId vertex_through = 0; vertex_through < vertex_count; ++vertex_through) {
        relax_routes_internal_data_through_vertex(vertex_count, vertex_through);
    }
}

template <typename Weight>
std::optional<typename Router<Weight>::RouteInfo> Router<Weight>::build_route(VertexId from,
                                                                              VertexId to) const {
    const auto& route_internal_data = routes_internal_data_.at(from).at(to);
    
    if (!route_internal_data) {
        return std::nullopt;
    }
    
    const Weight weight = route_internal_data->weight;
    std::vector<EdgeId> edges;
    for (std::optional<EdgeId> edge_id = route_internal_data->prev_edge;
         edge_id;
         edge_id = routes_internal_data_[from][graph_.get_edge(*edge_id).from]->prev_edge) {
        
        edges.push_back(*edge_id);
    }
    
    std::reverse(edges.begin(), edges.end());

    return RouteInfo{weight, std::move(edges)};
}

}//end namespace graph
-----------------------------------------------------------------------
serialization.cpp
-----------------------------------------------------------------------
#include "serialization.h"

namespace serialization {
    
template <typename It>
uint32_t calculate_id(It start, It end, std::string_view name) {

    auto stop_it = std::find_if(start, end, [&name](const domain::Stop stop) {return stop.name == name;});
    return std::distance(start, stop_it);
}

transport_catalogue_protobuf::TransportCatalogue transport_catalogue_serialization(const transport_catalogue::TransportCatalogue& transport_catalogue) {
    
    transport_catalogue_protobuf::TransportCatalogue transport_catalogue_proto;

    const auto& stops = transport_catalogue.get_stops();
    const auto& buses = transport_catalogue.get_buses();
    const auto& distances = transport_catalogue.get_distance();
    
    int id = 0;
    for (const auto& stop : stops) {

        transport_catalogue_protobuf::Stop stop_proto;

        stop_proto.set_id(id);
        stop_proto.set_name(stop.name);
        stop_proto.set_latitude(stop.latitude);
        stop_proto.set_longitude(stop.longitude);
        
        *transport_catalogue_proto.add_stops() = std::move(stop_proto);
        
        ++id;
    }

    for (const auto& bus : buses) {

        transport_catalogue_protobuf::Bus bus_proto;

        bus_proto.set_name(bus.name);

        for (auto stop : bus.stops) {
            uint32_t stop_id = calculate_id(stops.cbegin(), 
                                            stops.cend(), 
                                            stop->name);
            bus_proto.add_stops(stop_id);
        }

        bus_proto.set_is_roundtrip(bus.is_roundtrip);
        bus_proto.set_route_length(bus.route_length);

        *transport_catalogue_proto.add_buses() = std::move(bus_proto);
    }
    
    for (const auto& [pair_stops, pair_distance] : distances) {

        transport_catalogue_protobuf::Distance distance_proto;

        distance_proto.set_start(calculate_id(stops.cbegin(), 
                                              stops.cend(), 
                                              pair_stops.first->name));
                                              
        distance_proto.set_end(calculate_id(stops.cbegin(), 
                                            stops.cend(), 
                                            pair_stops.second->name));
                                            
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
        
        tc_stop.name = stop.name();
        tc_stop.latitude = stop.latitude();
        tc_stop.longitude = stop.longitude();
        
        transport_catalogue.add_stop(std::move(tc_stop));
    }
    
    const auto& tc_stops = transport_catalogue.get_stops(); 
    
    std::vector<domain::Distance> distances;
    for (const auto& distance : distances_proto) {
        
        domain::Distance tc_distance;
        
        tc_distance.start = transport_catalogue.get_stop(tc_stops[distance.start()].name);
        tc_distance.end = transport_catalogue.get_stop(tc_stops[distance.end()].name);
        
        tc_distance.distance = distance.distance();
        
        distances.push_back(tc_distance);
    }
    
    transport_catalogue.add_distance(distances);       
    
    for (const auto& bus_proto : buses_proto) {  
    
        domain::Bus tc_bus;
        
        tc_bus.name = bus_proto.name();

        for (auto stop_id : bus_proto.stops()) {
            auto name = tc_stops[stop_id].name;            
            tc_bus.stops.push_back(transport_catalogue.get_stop(name));
        }

        tc_bus.is_roundtrip = bus_proto.is_roundtrip();
        tc_bus.route_length = bus_proto.route_length();
        
        transport_catalogue.add_bus(std::move(tc_bus));
    }   
    
    return transport_catalogue;
}
    
transport_catalogue_protobuf::Color serialize_color(const svg::Color& tc_color) {
    
    transport_catalogue_protobuf::Color color_proto;
    
    if (std::holds_alternative<std::monostate>(tc_color)) {
        color_proto.set_none(true);
        
    } else if (std::holds_alternative<svg::Rgb>(tc_color)) {
        svg::Rgb rgb = std::get<svg::Rgb>(tc_color);
        
        color_proto.mutable_rgb()->set_red_(rgb.red_);
        color_proto.mutable_rgb()->set_green_(rgb.green_);
        color_proto.mutable_rgb()->set_blue_(rgb.blue_);
        
    } else if (std::holds_alternative<svg::Rgba>(tc_color)) {
        svg::Rgba rgba = std::get<svg::Rgba>(tc_color);
        
        color_proto.mutable_rgba()->set_red_(rgba.red_);
        color_proto.mutable_rgba()->set_green_(rgba.green_);
        color_proto.mutable_rgba()->set_blue_(rgba.blue_);
        color_proto.mutable_rgba()->set_opacity_(rgba.opacity_);
        
    } else if (std::holds_alternative<std::string>(tc_color)) {
        color_proto.set_string_color(std::get<std::string>(tc_color));
    }
    
    return color_proto;
}

svg::Color deserialize_color(const transport_catalogue_protobuf::Color& color_proto) {
    
    svg::Color color;
    
    if (color_proto.has_rgb()) {
        svg::Rgb rgb;
        
        rgb.red_ = color_proto.rgb().red_();
        rgb.green_ = color_proto.rgb().green_();
        rgb.blue_ = color_proto.rgb().blue_();
        
        color = rgb;
        
    } else if (color_proto.has_rgba()) {
        svg::Rgba rgba;
        
        rgba.red_ = color_proto.rgba().red_();
        rgba.green_ = color_proto.rgba().green_();
        rgba.blue_ = color_proto.rgba().blue_();
        rgba.opacity_ = color_proto.rgba().opacity_();
        
        color = rgba;
        
    } else {color = color_proto.string_color();}
    
    return color;
}
    
transport_catalogue_protobuf::RenderSettings render_settings_serialization(const map_renderer::RenderSettings& render_settings) {
    
    transport_catalogue_protobuf::RenderSettings render_settings_proto;
    
    render_settings_proto.set_width_(render_settings.width_);
    render_settings_proto.set_height_(render_settings.height_);
    render_settings_proto.set_padding_(render_settings.padding_);
    render_settings_proto.set_line_width_(render_settings.line_width_);
    render_settings_proto.set_stop_radius_(render_settings.stop_radius_);
    render_settings_proto.set_bus_label_font_size_(render_settings.bus_label_font_size_);

    transport_catalogue_protobuf::Point bus_label_offset_proto;   
    bus_label_offset_proto.set_x(render_settings.bus_label_offset_.first);
    bus_label_offset_proto.set_y(render_settings.bus_label_offset_.second);
    
    *render_settings_proto.mutable_bus_label_offset_() = std::move(bus_label_offset_proto);

    render_settings_proto.set_stop_label_font_size_(render_settings.stop_label_font_size_);

    transport_catalogue_protobuf::Point stop_label_offset_proto;
    stop_label_offset_proto.set_x(render_settings.stop_label_offset_.first);
    stop_label_offset_proto.set_y(render_settings.stop_label_offset_.second);
    
    *render_settings_proto.mutable_stop_label_offset_() = std::move(stop_label_offset_proto);
    *render_settings_proto.mutable_underlayer_color_() = std::move(serialize_color(render_settings.underlayer_color_));
    render_settings_proto.set_underlayer_width_(render_settings.underlayer_width_);
    
    const auto& colors = render_settings.color_palette_;
    for (const auto& color : colors) {
        *render_settings_proto.add_color_palette_() = std::move(serialize_color(color));
    }

    return render_settings_proto;
}
    
map_renderer::RenderSettings render_settings_deserialization(const transport_catalogue_protobuf::RenderSettings& render_settings_proto) {
    
    map_renderer::RenderSettings render_settings;
    
    render_settings.width_ = render_settings_proto.width_();
    render_settings.height_ = render_settings_proto.height_();
    render_settings.padding_ = render_settings_proto.padding_();
    render_settings.line_width_ = render_settings_proto.line_width_();
    render_settings.stop_radius_ = render_settings_proto.stop_radius_();
    render_settings.bus_label_font_size_ = render_settings_proto.bus_label_font_size_();
    
    render_settings.bus_label_offset_.first = render_settings_proto.bus_label_offset_().x();
    render_settings.bus_label_offset_.second = render_settings_proto.bus_label_offset_().y();
    
    render_settings.stop_label_font_size_ = render_settings_proto.stop_label_font_size_();
    
    render_settings.stop_label_offset_.first = render_settings_proto.stop_label_offset_().x();
    render_settings.stop_label_offset_.second = render_settings_proto.stop_label_offset_().y();
    
    render_settings.underlayer_color_ = deserialize_color(render_settings_proto.underlayer_color_());
    render_settings.underlayer_width_ = render_settings_proto.underlayer_width_();
    
    for (const auto& color_proto : render_settings_proto.color_palette_()) {
        render_settings.color_palette_.push_back(deserialize_color(color_proto));
    }
    
    return render_settings;
}  
    
void catalogue_serialization(transport_catalogue::TransportCatalogue& transport_catalogue, map_renderer::RenderSettings& render_settings, std::ostream& out) {
    
    transport_catalogue_protobuf::Catalogue catalogue_proto;

    transport_catalogue_protobuf::TransportCatalogue transport_catalogue_proto = transport_catalogue_serialization(transport_catalogue);
    transport_catalogue_protobuf::RenderSettings render_settings_proto = render_settings_serialization(render_settings);

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
-----------------------------------------------------------------------
serialization.h
-----------------------------------------------------------------------
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
    map_renderer::RenderSettings render_settings_;
};
    
template <typename It>
uint32_t calculate_id(It start, It end, std::string_view name);
    
transport_catalogue_protobuf::TransportCatalogue transport_catalogue_serialization(const transport_catalogue::TransportCatalogue& transport_catalogue);
transport_catalogue::TransportCatalogue transport_catalogue_deserialization(const transport_catalogue_protobuf::TransportCatalogue& transport_catalogue_proto);

transport_catalogue_protobuf::Color serialize_color(const svg::Color& tc_color);
svg::Color deserealize_color(const transport_catalogue_protobuf::Color& color_proto);
transport_catalogue_protobuf::RenderSettings render_settings_serialization(const map_renderer::RenderSettings& render_settings);
map_renderer::RenderSettings render_settings_deserialization(const transport_catalogue_protobuf::RenderSettings& render_settings_proto);

void catalogue_serialization(transport_catalogue::TransportCatalogue& transport_catalogue, 
                             map_renderer::RenderSettings& render_settings, 
                             std::ostream& out);
    
Catalogue catalogue_deserialization(std::istream& in);
    
}//end namespace serialization
-----------------------------------------------------------------------
svg.cpp
-----------------------------------------------------------------------
#include "svg.h"

namespace svg {

using namespace std::literals; 

Rgb::Rgb(uint8_t red, uint8_t green, uint8_t blue) : red_(red)
                                                   , green_(green)
                                                   , blue_(blue) {}  
    
Rgba::Rgba(uint8_t red, uint8_t green, uint8_t blue, double opacity) : red_(red)
                                                                     , green_(green)
                                                                     , blue_(blue)
                                                                     , opacity_(opacity) {}
    
inline void print_color(std::ostream& out, Rgb& rgb) {
    out << "rgb("sv << static_cast<short>(rgb.red_) << ","sv
                    << static_cast<short>(rgb.green_) << ","sv 
                    << static_cast<short>(rgb.blue_) << ")"sv;
}
    
inline void print_color(std::ostream& out, Rgba& rgba) {
    out << "rgba("sv << static_cast<short>(rgba.red_) << ","sv 
                     << static_cast<short>(rgba.green_) << ","sv 
                     << static_cast<short>(rgba.blue_) << ","sv 
                     << (rgba.opacity_) << ")"sv;
}
    
inline void print_color(std::ostream& out, std::monostate) {
    out << "none"sv;
}
 
inline void print_color(std::ostream& out, std::string& color) {
    out << color;
}
    
std::ostream& operator<<(std::ostream& out, const Color& color) {
    std::visit([&out](auto value) {
            print_color(out, value);
    }, color);
    
    return out;
} 
    
RenderContext::RenderContext(std::ostream& out) : out_(out) {}
    
RenderContext RenderContext::indented() const {
        return {out_, 
                indent_step_, 
                indent_ + indent_step_};
}
    
void RenderContext::render_indent() const {
        for (int i = 0; i < indent_; ++i) {
            out_.put(' ');
        }
}

void Object::render(const RenderContext& context) const {
    context.render_indent();
    render_object(context);
    context.out_ << std::endl;
}

Circle& Circle::set_center(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::set_radius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::render_object(const RenderContext& context) const {
    std::ostream& out = context.out_;

    out << "<circle cx=\""sv << center_.x 
        << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    
    render_attrs(context.out_);
    out << "/>"sv;
}
    
Polyline& Polyline::add_point(Point point) {
    points_.emplace_back(point);
    return *this;
}
 
void Polyline::render_object(const RenderContext& context) const {
    
    std::ostream& out = context.out_;
    out << "<polyline points=\"";
    
    for (size_t i = 0; i < points_.size(); ++i) {
        out << points_[i].x << ","sv << points_[i].y;
 
        if (i+1 != points_.size()) {
            out << " "sv;
        }
    }
    out << "\" "; 
    render_attrs(context.out_);
    out << "/>";
}
    
Text& Text::set_position(Point pos) {
    position_ = pos;
    return *this;
}
 
Text& Text::set_offset(Point offset) {
    offset_ = offset;
    return *this;
}
    
Text& Text::set_font_size(uint32_t size) {
    font_size_ = size;
    return *this;
}
 
Text& Text::set_font_family(std::string font_family) {
    font_family_ = std::move(font_family);
    return *this;
}
 
Text& Text::set_font_weight(std::string font_weight) {
    font_weight_ = std::move(font_weight);
    return *this;
}
 
Text& Text::set_data(std::string data) {
    data_ = std::move(data);
    return *this;
}
 
std::string Text::delete_spaces(const std::string& str) {
    if (str.empty()) {
        return {};
    } else {
        
        auto left = str.find_first_not_of(' ');
        auto right = str.find_last_not_of(' ');
        return str.substr(left, right - left + 1);  
    }
}
 
std::string Text::uniq_symbols(const std::string& str) {
    
    std::string out;
 
    for (char ch : str) {
        
        if (ch == '"') {
            out += "&quot;"sv;
            continue;
            
        } else if (ch == '`' || ch == '\'') {
            out += "&apos;"sv;
            continue;
            
        } else if (ch == '<') {
            out += "&lt;"sv;
            continue;
            
        } else if (ch == '>') {
            out += "&gt;"sv;
            continue;
            
        } else if (ch == '&') {
            out += "&amp;"sv;
            continue;
            
        } else {
            
        }
        
        out += ch;
    }
 
    return out;
}
 
void Text::render_object(const RenderContext& context) const {
    
    std::ostream& out = context.out_;
    out << "<text "; 
    render_attrs(context.out_);
    out << "x=\"" 
        << position_.x << "\" y=\"" 
        << position_.y << "\" "
        << "dx=\"" 
        << offset_.x << "\" dy=\"" 
        << offset_.y << "\" "
        << "font-size=\"" 
        << font_size_ << "\" ";
 
    if (!font_family_.empty()) {
        out << "font-family=\"" << font_family_ << "\" ";
    }
 
    if (!font_weight_.empty()) {
        out << "font-weight=\"" << font_weight_ << "\"";
    }

    out << ">"sv << delete_spaces(uniq_symbols(data_)) << "</text>"sv;

}

void Document::render(std::ostream& out) const{
    int indent = 2;
    int indent_step = 2;
 
    RenderContext context(out, indent_step, indent);
 
    const std::string_view xml = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv;
    const std::string_view svg = "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv;
 
    out << xml << "\n"sv << svg << "\n"sv;
 
    for (const auto& object : objects_) {
        object->render(context);
    }
    
    out << "</svg>"sv;
}
 
}//end namespace svg
-----------------------------------------------------------------------
svg.h
-----------------------------------------------------------------------
#pragma once
#define _USE_MATH_DEFINES

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <cmath>
#include <variant>

namespace svg {

class Rgb {
public:
    Rgb() = default;
    Rgb(uint8_t red, uint8_t green, uint8_t blue);
    
    uint8_t red_ = 0;
    uint8_t green_ = 0;
    uint8_t blue_ = 0;
};
inline void print_color(std::ostream& out, Rgb& rgb);
 
class Rgba {
public:
    Rgba() = default;
    Rgba(uint8_t red, uint8_t green, uint8_t blue, double opacity);
    
    uint8_t red_ = 0;
    uint8_t green_ = 0;
    uint8_t blue_ = 0;
    double opacity_ = 1.0;
};  
inline void print_color(std::ostream& out, Rgba& rgba);

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;
inline const Color NoneColor{"none"};  
    
inline void print_color(std::ostream& out, std::monostate);
inline void print_color(std::ostream& out, std::string& color);
std::ostream& operator<<(std::ostream& out, const Color& color); 
    
enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};
    
inline std::ostream &operator<<(std::ostream &out, StrokeLineCap stroke_line_cap) {
    using namespace std::literals;
    
    if (stroke_line_cap == StrokeLineCap::BUTT) {
        out << "butt"sv;
    } else if (stroke_line_cap == StrokeLineCap::ROUND) {
        out << "round"sv;
    } else if (stroke_line_cap == StrokeLineCap::SQUARE) {
        out << "square"sv;
    }
    return out;
}
 
enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};
    
inline std::ostream &operator<<(std::ostream &out, StrokeLineJoin stroke_line_join) {
    using namespace std::literals;
    
    if (stroke_line_join == StrokeLineJoin::ARCS) {
        out << "arcs"sv;
    } else if (stroke_line_join == StrokeLineJoin::BEVEL) {
        out << "bevel"sv;
    } else if (stroke_line_join == StrokeLineJoin::MITER) {
        out << "miter"sv;
    } else if (stroke_line_join == StrokeLineJoin::MITER_CLIP) {
        out << "miter-clip"sv;
    } else if (stroke_line_join == StrokeLineJoin::ROUND) {
        out << "round"sv;
    }
    return out;
}
    
template<typename Owner>
class PathProps {
public:
    Owner& set_fill_color(const Color& color) {
        fill_color_ = std::move(color);
        return as_owner();
    }
 
    Owner& set_stroke_color(const Color& color) {
        stroke_color_ = std::move(color);
        return as_owner();
    }
 
    Owner& set_stroke_width(double width) {
        stroke_width_ = width;
        return as_owner();
    }
 
    Owner& set_stroke_linecap(StrokeLineCap line_cap) {
        stroke_line_cap_ = line_cap;
        return as_owner();
    }
 
    Owner& set_stroke_linejoin(StrokeLineJoin line_join) {
        stroke_line_join_ = line_join;
        return as_owner();
    } 
 
protected:
    ~PathProps() = default;
 
    void render_attrs(std::ostream &out) const {
        using namespace std::literals;
 
        if (fill_color_ != std::nullopt) {
            out << "fill=\""sv << *fill_color_ << "\" "sv;
        }
        if (stroke_color_ != std::nullopt) {
            out << "stroke=\""sv << *stroke_color_ << "\" "sv;
        }
        if (stroke_width_ != std::nullopt) {
            out << "stroke-width=\""sv << *stroke_width_ << "\" "sv;
        }
        if (stroke_line_cap_ != std::nullopt) {
            out << "stroke-linecap=\""sv << *stroke_line_cap_ << "\" "sv;
        }
        if (stroke_line_join_ != std::nullopt) {
            out << "stroke-linejoin=\""sv << *stroke_line_join_ << "\" "sv;
        }
    }
    
private: 
    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> stroke_width_;
    std::optional<StrokeLineCap> stroke_line_cap_;
    std::optional<StrokeLineJoin> stroke_line_join_;
    
    Owner& as_owner() {
        return static_cast<Owner&>(*this);
    }
};
    
struct Point {
    Point() = default;
    Point(double x, double y) : x(x)
                              , y(y) {}
    double x = 0;
    double y = 0;
};

struct RenderContext {
    RenderContext(std::ostream& out);
    RenderContext(std::ostream& out, int indent_step, int indent = 0) : out_(out)
                                                                      , indent_step_(indent_step)
                                                                      , indent_(indent) {}
    RenderContext indented() const;
    void render_indent() const;

    std::ostream& out_;
    int indent_step_ = 0;
    int indent_ = 0;
};

class Object {
public:
    void render(const RenderContext& context) const;
    virtual ~Object() = default;

private:
    virtual void render_object(const RenderContext& context) const = 0;
};

class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& set_center(Point center);
    Circle& set_radius(double radius);

private:
    Point center_;
    double radius_ = 1.0;
    
    void render_object(const RenderContext& context) const override;
};

class Polyline final : public Object, public PathProps<Polyline> {
public:
    Polyline& add_point(Point point);
    
private:
    std::vector<Point> points_;
    void render_object(const RenderContext& context) const override;
};

class Text final : public Object, public PathProps<Text> {
public:
    Text& set_position(Point pos);
    Text& set_offset(Point offset);
    Text& set_font_size(uint32_t size);
    Text& set_font_family(std::string font_family);
    Text& set_font_weight(std::string font_weight);
    Text& set_data(std::string data);

private:
    Point position_;
    Point offset_;
    std::string font_family_;
    std::string font_weight_;
    uint32_t font_size_ = 1;
    std::string data_;
     
    static std::string delete_spaces(const std::string& str);
    static std::string uniq_symbols(const std::string& str);
    
    void render_object(const RenderContext& context) const override;
};

class ObjectContainer {
public:
    virtual ~ObjectContainer() = default;
 
    template<typename Obj>
    void add(Obj obj);
 
    virtual void add_ptr(std::unique_ptr<Object> &&) = 0;
 
protected:
    std::vector<std::unique_ptr<Object>> objects_;
};
    
template<typename Obj>
void ObjectContainer::add(Obj obj) {
    objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
}
    
class Drawable {
public:
    virtual void draw(ObjectContainer& container) const = 0;
    virtual ~Drawable() = default;
};
    
class Document : public ObjectContainer {
public: 
    
    void add_ptr(std::unique_ptr<Object>&& obj) override {
        objects_.emplace_back(std::move(obj));
    }
    
    void render(std::ostream& out) const;
    
};
    
}//end namespace svg
-----------------------------------------------------------------------
transport_catalogue.cpp
-----------------------------------------------------------------------
#include "transport_catalogue.h"

namespace transport_catalogue {  
    
void TransportCatalogue::add_stop(Stop&& stop) {
    stops.push_back(std::move(stop));
    Stop* stop_buf = &stops.back();
    stopname_to_stop.insert(transport_catalogue::StopMap::value_type(stop_buf->name, stop_buf));
}

void TransportCatalogue::add_bus(Bus&& bus) {
    Bus* bus_buf;
    
    buses.push_back(std::move(bus)); 
    bus_buf = &buses.back();
    busname_to_bus.insert(BusMap::value_type(bus_buf->name, bus_buf));

    for (Stop* stop : bus_buf->stops) {
         stop->buses.push_back(bus_buf);
    }
    
    bus_buf->route_length = get_distance_to_bus(bus_buf);
}

void TransportCatalogue::add_distance(const std::vector<Distance>& distances) {
    
    for (auto distance : distances) {
        auto dist_pair = std::make_pair(distance.start, distance.end);
        distance_to_stop.insert(DistanceMap::value_type(dist_pair, distance.distance));
    }
}

Bus* TransportCatalogue::get_bus(std::string_view bus_name) {
    if (busname_to_bus.empty()) {
        return nullptr;
    }
    
    if (busname_to_bus.count(bus_name)) {
        return busname_to_bus.at(bus_name);
    } else {
        return nullptr;
    }
}
    
Stop* TransportCatalogue::get_stop(std::string_view stop_name) {
    if (stopname_to_stop.empty()) {
        return nullptr;
    }
    
    if (stopname_to_stop.count(stop_name)) {
        return stopname_to_stop.at(stop_name);
    } else {
        return nullptr;
    }
}
    
std::deque<Stop> TransportCatalogue::get_stops() const {
    return stops;
}
    
std::deque<Bus> TransportCatalogue::get_buses() const {
    return buses;
}
    
BusMap TransportCatalogue::get_busname_to_bus() const {
    return busname_to_bus;
}
    
StopMap TransportCatalogue::get_stopname_to_stop() const {
    return stopname_to_stop;
}

std::unordered_set<const Stop*> TransportCatalogue::get_uniq_stops(Bus* bus) {
    std::unordered_set<const Stop*> unique_stops;   
    unique_stops.insert(bus->stops.begin(), bus->stops.end());
    
    return unique_stops;
}
    
double TransportCatalogue::get_length(Bus* bus) {
    return transform_reduce(next(bus->stops.begin()), 
                            bus->stops.end(), 
                            bus->stops.begin(),
                            0.0,
                            std::plus<>{},
                            [](const Stop* lhs, const Stop* rhs) { 
                                return geo::compute_distance({(*lhs).latitude,
                                                              (*lhs).longitude}, {(*rhs).latitude,
                                                                                  (*rhs).longitude});
                            });
}

std::unordered_set<const Bus*> TransportCatalogue::stop_get_uniq_buses(Stop* stop){    
    std::unordered_set<const Bus*> unique_stops;          
    unique_stops.insert(stop->buses.begin(), stop->buses.end());   
    
    return unique_stops;
}
    
DistanceMap TransportCatalogue::get_distance() const {
    return distance_to_stop;
}

size_t TransportCatalogue::get_distance_stop(const Stop* begin, const Stop* finish) const {
    
    if (distance_to_stop.empty()) {
        return 0;
        
    } else {
        
        if (const auto& stop_ptr_pair = std::make_pair(begin, finish);
            distance_to_stop.count(stop_ptr_pair)) {
            
            return distance_to_stop.at(stop_ptr_pair);

        } else if (const auto& stop_ptr_pair = std::make_pair(finish, begin);
                   distance_to_stop.count(stop_ptr_pair)) {
            
            return distance_to_stop.at(stop_ptr_pair);
            
        } else {
            
            return 0;
        } 
    }
}

size_t TransportCatalogue::get_distance_to_bus(Bus* bus) {
    size_t distance = 0;
    auto stops_size = bus->stops.size() - 1;
    
    for (int i = 0; i < static_cast<int>(stops_size); i++) {
        distance += get_distance_stop(bus->stops[i], bus->stops[i+1]);
    }
    
    return distance;
}
    
}//end namespace transport_catalogue
-----------------------------------------------------------------------
transport_catalogue.h
-----------------------------------------------------------------------
#pragma once
#include <deque>
#include <string>
#include <vector>
#include <iomanip>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <numeric>

#include "domain.h"

using namespace domain;

namespace transport_catalogue {   

struct DistanceHasher {
    std::hash<const void*> hasher;
    
    std::size_t operator()(const std::pair<const Stop*, const Stop*> pair_stops) const noexcept {
        auto hash_1 = static_cast<const void*>(pair_stops.first);
        auto hash_2 = static_cast<const void*>(pair_stops.second);
        return hasher(hash_1) * 17 + hasher(hash_2);
    }  
};
    
typedef  std::unordered_map<std::string_view, Stop*> StopMap;
typedef  std::unordered_map<std::string_view, Bus*> BusMap;
typedef  std::unordered_map<std::pair<const Stop*, const  Stop*>, int, DistanceHasher> DistanceMap;

class TransportCatalogue {
public:      
    void add_bus(Bus&& bus);
    void add_stop(Stop&& stop);
    void add_distance(const std::vector<Distance>& distances);
    
    Bus* get_bus(std::string_view bus_name);
    Stop* get_stop(std::string_view stop_name);
    
    std::deque<Stop> get_stops() const;
    std::deque<Bus> get_buses() const;
    
    BusMap get_busname_to_bus() const;
    StopMap get_stopname_to_stop() const;
    
    std::unordered_set<const Bus*> stop_get_uniq_buses(Stop* stop);    
    std::unordered_set<const Stop*> get_uniq_stops(Bus* bus);
    double get_length(Bus* bus);
    
    DistanceMap get_distance() const;
    size_t get_distance_stop(const Stop* start, const Stop* finish) const;
    size_t get_distance_to_bus(Bus* bus);
    
private:    
    std::deque<Stop> stops;
    StopMap stopname_to_stop;
    
    std::deque<Bus> buses;
    BusMap busname_to_bus;
    
    DistanceMap distance_to_stop;
};
    
}//end namespace transport_catalogue
-----------------------------------------------------------------------
transport_catalogue.proto
-----------------------------------------------------------------------
syntax = "proto3";

import "map_renderer.proto";

package transport_catalogue_protobuf;

message Stop {
    uint32 id = 1;
	string name = 2;
	double latitude = 3;
	double longitude = 4;
}

message Bus {
    string name = 1;
    repeated uint32 stops = 2;
	bool is_roundtrip = 3;	
    uint32 route_length = 4;
}

message Distance {
    uint32 start = 1;
    uint32 end = 2;
    uint32 distance = 3;
}

message TransportCatalogue {
    repeated Stop stops = 1;
    repeated Bus buses = 2;
    repeated Distance distances = 3;
}

message Catalogue {
    TransportCatalogue transport_catalogue = 1;
    RenderSettings render_settings = 2;
}
-----------------------------------------------------------------------
transport_router.cpp
-----------------------------------------------------------------------
#include "transport_router.h"

namespace transport_catalogue {
namespace detail {
namespace router {

void TransportRouter::set_routing_settings(RoutingSettings routing_settings) {routing_settings_ = std::move(routing_settings);}
const RoutingSettings& TransportRouter::get_routing_settings() const {return routing_settings_;}

void TransportRouter::build_router(TransportCatalogue& transport_catalogue) {
	set_graph(transport_catalogue);
	router_ = std::make_unique<Router<double>>(*graph_);
	router_->build();
}

const DirectedWeightedGraph<double>& TransportRouter::get_graph() const {return *graph_;}
const Router<double>& TransportRouter::get_router() const {return *router_;}
const std::variant<StopEdge, BusEdge>& TransportRouter::get_edge(EdgeId id) const {return edge_id_to_edge_.at(id);}

std::optional<RouterByStop> TransportRouter::get_router_by_stop(Stop* stop) const {
	if (stop_to_router_.count(stop)) {
        return stop_to_router_.at(stop);
	} else {
        return std::nullopt;
    }
}

std::optional<RouteInfo> TransportRouter::get_route_info(VertexId start, graph::VertexId end) const {
	const auto& route_info = router_->build_route(start, end);
	if (route_info) {
        RouteInfo result;
        result.total_time = route_info->weight;
        
        for (const auto edge : route_info->edges) {
            result.edges.emplace_back(get_edge(edge));
        }
        
        return result;
        
	} else {
        return std::nullopt;
    }
}   
    
const std::unordered_map<Stop*, RouterByStop>& TransportRouter::get_stop_to_vertex() const {return stop_to_router_;}
const std::unordered_map<EdgeId, std::variant<StopEdge, BusEdge>>& TransportRouter::get_edge_id_to_edge() const {return edge_id_to_edge_;}
    
std::deque<Stop*> TransportRouter::get_stops_ptr(TransportCatalogue& transport_catalogue) {
    std::deque<Stop*> stops_ptr;  
    
    for (auto [_, stop_ptr] : transport_catalogue.get_stopname_to_stop()) {
        stops_ptr.push_back(stop_ptr);
    }

    return stops_ptr;
}
    
std::deque<Bus*> TransportRouter::get_bus_ptr(TransportCatalogue& transport_catalogue) {
    std::deque<Bus*> buses_ptr;  
    
    for (auto [_, bus_ptr] : transport_catalogue.get_busname_to_bus()) {
        buses_ptr.push_back(bus_ptr);
    }

    return buses_ptr;
}
    
void TransportRouter::set_stops(const std::deque<Stop*>& stops) {
	size_t i = 0;
    
	for (const auto stop : stops) {
		VertexId first = i++;
		VertexId second = i++;
        
		stop_to_router_[stop] = RouterByStop{first, second};
	}
}

void TransportRouter::add_edge_to_stop() {   
    
	for (const auto [stop, num] : stop_to_router_) {
		EdgeId id = graph_->add_edge(Edge<double>{num.bus_wait_start, 
                                                 num.bus_wait_end, 
                                                 routing_settings_.bus_wait_time});
        
		edge_id_to_edge_[id] = StopEdge{stop->name, routing_settings_.bus_wait_time};
	}
}

void TransportRouter::add_edge_to_bus(TransportCatalogue& transport_catalogue) {

	for (auto bus : get_bus_ptr(transport_catalogue)) {        
		parse_bus_to_edges(bus->stops.begin(), 
                           bus->stops.end(), 
                           transport_catalogue,
                           bus);
        
		if (!bus->is_roundtrip) {
			parse_bus_to_edges(bus->stops.rbegin(), 
                               bus->stops.rend(), 
                               transport_catalogue,
                               bus);
		}
	}
}

void TransportRouter::set_graph(TransportCatalogue& transport_catalogue) {
    const auto stops_ptr_size = get_stops_ptr(transport_catalogue).size();  
    
	graph_ = std::make_unique<DirectedWeightedGraph<double>>(2 * stops_ptr_size);    
    
	set_stops(get_stops_ptr(transport_catalogue));    
	add_edge_to_stop();
	add_edge_to_bus(transport_catalogue);
}

Edge<double> TransportRouter::make_edge_to_bus(Stop* start, Stop* end, const double distance) const {
	Edge<double> result;
    
	result.from = stop_to_router_.at(start).bus_wait_end;
	result.to = stop_to_router_.at(end).bus_wait_start;
	result.weight = distance * 1.0 / (routing_settings_.bus_velocity * KILOMETER / HOUR);
    
	return result;
}

}//end namespace router
}//end namespace detail
}//end namespace transport_catalogue
-----------------------------------------------------------------------
transport_router.h
-----------------------------------------------------------------------
#pragma once

#include "transport_catalogue.h"
#include "router.h"
#include "domain.h"

#include <deque>
#include <unordered_map>
#include <iostream>

namespace transport_catalogue {
namespace detail {
namespace router {

using namespace domain;
using namespace graph;
    
static const uint16_t KILOMETER = 1000;
static const uint16_t HOUR = 60;    

class TransportRouter {
public:
	void set_routing_settings(RoutingSettings routing_settings);
	const RoutingSettings& get_routing_settings() const;

	void build_router(TransportCatalogue& transport_catalogue);

	const DirectedWeightedGraph<double>& get_graph() const;
    const Router<double>& get_router() const;
    const std::variant<StopEdge, BusEdge>& get_edge(EdgeId id) const;
    
	std::optional<RouterByStop> get_router_by_stop(Stop* stop) const;
	std::optional<RouteInfo> get_route_info(VertexId start, VertexId end) const;

	const std::unordered_map<Stop*, RouterByStop>& get_stop_to_vertex() const;
	const std::unordered_map<EdgeId, std::variant<StopEdge, BusEdge>>& get_edge_id_to_edge() const;
    
    std::deque<Stop*> get_stops_ptr(TransportCatalogue& transport_catalogue);
    std::deque<Bus*> get_bus_ptr(TransportCatalogue& transport_catalogue);
        
	void add_edge_to_stop();
	void add_edge_to_bus(TransportCatalogue& transport_catalogue);
    
    void set_stops(const std::deque<Stop*>& stops);
    void set_graph(TransportCatalogue& transport_catalogue);

	Edge<double> make_edge_to_bus(Stop* start, Stop* end, const double distance) const;

	template <typename Iterator>
	void parse_bus_to_edges(Iterator first, 
                            Iterator last,
                            const TransportCatalogue& transport_catalogue, 
                            const Bus* bus);
    
private:    
    std::unordered_map<Stop*, RouterByStop> stop_to_router_;
	std::unordered_map<EdgeId, std::variant<StopEdge, BusEdge>> edge_id_to_edge_;
    
	std::unique_ptr<DirectedWeightedGraph<double>> graph_;
	std::unique_ptr<Router<double>> router_;
    
    RoutingSettings routing_settings_;
};

template <typename Iterator>
void TransportRouter::parse_bus_to_edges(Iterator first, 
                                         Iterator last,
                                         const TransportCatalogue& transport_catalogue, 
                                         const Bus* bus) {
    
    for (auto it = first; it != last; ++it) {
        size_t distance = 0;
        size_t span = 0;

        for (auto it2 = std::next(it); it2 != last; ++it2) {
            distance += transport_catalogue.get_distance_stop(*prev(it2), *it2);
            ++span;

            EdgeId id = graph_->add_edge(make_edge_to_bus(*it, *it2, distance));
            
            edge_id_to_edge_[id] = BusEdge{bus->name, span, graph_->get_edge(id).weight};
        }
    }
}

}//end namespace router
}//end namespace detail
}//end namespace transport_catalogue
-----------------------------------------------------------------------
map_renderer.proto
-----------------------------------------------------------------------
syntax = "proto3";

import "svg.proto";

package transport_catalogue_protobuf;

message RenderSettings {
    double width_ = 1;
    double height_ = 2;
    double padding_ = 3;
    double line_width_ = 4;
    double stop_radius_ = 5;
    uint32 bus_label_font_size_ = 6;
    Point bus_label_offset_ = 7;
    uint32 stop_label_font_size_ = 8;
    Point stop_label_offset_ = 9;
    Color underlayer_color_ = 10;
    double underlayer_width_ = 11;
    repeated Color color_palette_ = 12;
}
-----------------------------------------------------------------------
svg.proto
-----------------------------------------------------------------------
syntax = "proto3";

package transport_catalogue_protobuf;

message Point {
    double x = 1;
    double y = 2;
}

message Rgb {
    uint32 red_ = 1;
    uint32 green_ = 2;
    uint32 blue_ = 3;
}

message Rgba {
    uint32 red_ = 1;
    uint32 green_ = 2;
    uint32 blue_ = 3;
    double opacity_ = 4;
}

message Color {
    oneof colors {
        Rgb rgb = 1;
        Rgba rgba = 2;
        string string_color = 3;
        
        bool none = 4;
    }
}
