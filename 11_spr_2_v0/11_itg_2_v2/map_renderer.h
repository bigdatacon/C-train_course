#pragma once

#include "geo.h"
#include "svg.h"
#include "request_handler.h"
#include "transport_catalogue.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>

#include <variant>
#include <array>
#include <string>
#include <map>
#include <deque>

using namespace std;

using namespace transport_catalogue;
using namespace svg;

struct Render_data {
    Render_data() = default;
    uint16_t red = 0;
    uint16_t green = 0;
    uint16_t blue = 0;
    double width = 1200.0;
    double height = 1200.0;
    double padding = 50.0;
    double line_width = 14.0;
    double stop_radius = 5.0;
    int bus_label_font_size = 20;
    std::array<double, 2> bus_label_offset = { 7.0, 15.0 };
    int stop_label_font_size = 20;
    std::array<double, 2> stop_label_offset = { 7.0, -3.0 };
    std::array<int, 4> underlayer_color = { 255, 255, 255, 85 };
    double underlayer_width = 3.0;
    std::vector<std::variant<std::string, std::vector<int>>> color_palette{
        std::initializer_list<std::variant<std::string, std::vector<int>>> {
            "green",
            std::vector<int> { 255, 160, 0 },
            "red"
        }
    };
};




inline const double EPSILON = 1e-6;
bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

class SphereProjector {
public:
    // points_begin � points_end ������ ������ � ����� ��������� ��������� geo::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
        double max_width, double max_height, double padding)
        : padding_(padding) //
    {
        // ���� ����� ����������� ����� �� ������, ��������� ������
        if (points_begin == points_end) {
            return;
        }

        // ������� ����� � ����������� � ������������ ��������
        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // ������� ����� � ����������� � ������������ �������
        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // ��������� ����������� ��������������� ����� ���������� x
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        // ��������� ����������� ��������������� ����� ���������� y
        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            // ������������ ��������������� �� ������ � ������ ���������,
            // ���� ����������� �� ���
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        }
        else if (width_zoom) {
            // ����������� ��������������� �� ������ ���������, ���������� ���
            zoom_coeff_ = *width_zoom;
        }
        else if (height_zoom) {
            // ����������� ��������������� �� ������ ���������, ���������� ���
            zoom_coeff_ = *height_zoom;
        }
    }

    // ���������� ������ � ������� � ���������� ������ SVG-�����������
    svg::Point operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};


std::map<string, variant<std::string, std::vector<int>>> GetColorForRoute(std::deque<transport_catalogue::Bus> buses, std::vector<std::variant<std::string, std::vector<int>>> color_palette) {
    std::map<string, variant<std::string, std::vector<int>>> result;
    int index_color = 0;
    for (auto bus : buses) {
        if (bus.stops.size() != 0) {
            if (index_color > color_palette.size() - 1) { index_color = 0; }

            result[bus.bus_name] = color_palette[index_color];
            ++index_color;
        }
    
    }
    return result;

}


deque<string*> GetStopsForNonRounTtip(deque<string*> stops) {
    deque<string*> reverse_stops;
    reverse_stops.insert(reverse_stops.end(), stops.begin(), stops.end());
    std::reverse(stops.begin(), stops.end());
    stops.pop_front();
    reverse_stops.insert(reverse_stops.end(), stops.begin(), stops.end());
 
    return stops;

}

class RouteLine : public svg::Drawable {
public:
    RouteLine(Render_data render_data, vector<svg::Point> points, variant<std::string, std::vector<int>> current_color)
        : render_data_(render_data),
        current_color_(current_color)
    {
        for (auto p : points) { polyline_.AddPoint(p); }
    }

    // ��������� ����� Draw ���������� svg::Drawable
    void Draw(svg::ObjectContainer& container) const override {
        //container.AddPtr(std::make_unique<Polyline>(::CreateStar(p1_, outer_radius_, inner_radius_, num_rays_)));
        container.AddPtr(  //
            std::make_unique<Polyline>(
                polyline_.SetFillColor("red"s).SetStrokeColor("black"s)));
    }

private:
    Render_data render_data_;
    variant<std::string, std::vector<int>> current_color_;
    svg::Polyline polyline_;
};

void DrawRoute(/*transport_catalogue::TransportCatalogue tc*/  RequestToTc rtotc) { // ������� ��� ��� �������� ����� �� ������ ��������, ������ ����� ������������� ����� �������� 
    const double WIDTH = 600.0;
    const double HEIGHT = 400.0;
    const double PADDING = 50.0;
    
    vector<svg::Document> docs; // ������ �������� ������� ���������� ���� ����� �������� 

    Render_data render_data;
    

    std::vector<std::variant<std::string, std::vector<int>>> color_palette = r.color_palette;
    std::deque<transport_catalogue::Bus> buses = rtotc.GetBuses();
    std::deque<transport_catalogue::Stop> stops = rtotc.GetStops();
    std::sort(buses.begin(), buses.end(),
        [](const transport_catalogue::Bus& a, const transport_catalogue::Bus& b) { return a.bus_name < b.bus_name; });
    //0 ��������� ���� �������� 
    std::map<string, variant<std::string, std::vector<int>>> colors = GetColorForRoute(buses, color_palette);

    //1 ��� ������� �������� ������� ������ ���������
    for (auto bus : buses) {

        vector<geo::Coordinates> geo_coords;


        // a) ���� �������� �������� 
        variant<std::string, std::vector<int>> current_color;
        deque<string*> current_stops;
        if (colors.count(bus.bus_name)) {
            variant<std::string, std::vector<int>> current_color = colors[bus.bus_name];
        }

        //�) ��������� �������� ��������� r
        if (bus.type == "true") {
            current_stops = GetStopsForNonRounTtip(bus.stops);
        }
        else { current_stops = bus.stops; }

        //�) ��� �� ������ ��������� � ������ ��� ��� ����������. �������� ���������� � ������ 
        for (int i = 0; i < current_stops.size() - 1; i++) {
            const Stop* one = rtotc.FindStop(*current_stops[i]);
            geo_coords.push_back(one->coordinates);
        }
        //� ������ �������� ����������� ��������� �� �����
        const SphereProjector proj{
            geo_coords.begin(), geo_coords.end(), WIDTH, HEIGHT, PADDING
        };

        vector<svg::Point> point_to_draw;

        //� ���������� � ������� ����������
        for (const auto geo_coord : geo_coords) {
            //�) ������� ����� �������� ��������� 
            const svg::Point screen_coord = proj(geo_coord);
            cout << '(' << geo_coord.lat << ", "sv << geo_coord.lng << ") -> "sv;
            cout << '(' << screen_coord.x << ", "sv << screen_coord.y << ')' << endl;
            svg::Point p;
            p.x = screen_coord.x;
            p.y = screen_coord.y;
            point_to_draw.push_back(p);
        }
        
        // � ����� ���� �������� ��������� : current_stops, ���� ��������, ����� ���������� -���� ������ ���� ����������
        // �������� ����� ����� 
        svg::Document  doc;
        RouteLine routline(render_data, point_to_draw, current_color);
        doc.Add(std::move(routline));
        docs.push_back(doc);
    }
    // ����� ����� 
    for (auto doc : docs) { doc.Render(cout); }
    


}




/*
int main() {
    using namespace std;

    const double WIDTH = 600.0;
    const double HEIGHT = 400.0;
    const double PADDING = 50.0;

    // �����, ���������� �������������
    vector<geo::Coordinates> geo_coords = {
        {43.587795, 39.716901}, {43.581969, 39.719848}, {43.598701, 39.730623},
        {43.585586, 39.733879}, {43.590317, 39.746833}
    }; 

    // ������ �������� ����������� ��������� �� �����
    const SphereProjector proj{
        geo_coords.begin(), geo_coords.end(), WIDTH, HEIGHT, PADDING
    };

    // ���������� � ������� ����������
    for (const auto geo_coord : geo_coords) {
        const svg::Point screen_coord = proj(geo_coord);
        cout << '(' << geo_coord.lat << ", "sv << geo_coord.lng << ") -> "sv;
        cout << '(' << screen_coord.x << ", "sv << screen_coord.y << ')' << endl;
    }
}
*/