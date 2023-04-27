#pragma once
#define _USE_MATH_DEFINES
#include "svg.h" // ���������� ������� ���������� ������ ���� ����������� � ����� svg.h

#include <cmath>
#include "geo.h"
#include "svg.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>

#include "request_handler.h"
#include "transport_catalogue.h"
//#include "json_reader.h"
#include <variant>
#include <array>
#include <string>
#include <map>
#include <deque>


using namespace std::literals;
using namespace svg;
using namespace std;
using namespace transport_catalogue;

struct Render_data {
    //Render_data() = default;
    double width;
    double height;
    double padding;
    double line_width;
    double stop_radius;
    int bus_label_font_size;
    std::vector<double> bus_label_offset;
    int stop_label_font_size;
    std::vector<double> stop_label_offset;
    Rgba underlayer_color;
    double underlayer_width;
    std::vector<Color> color_palette;

};

/*
struct Render_data {
    Render_data() = default;
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
    std::vector<Color> color_palette{
            "green",
            Rgb{ 255, 160, 0 },
            "red"
    };

};
*/


svg::Polyline CreatePolyline(vector<svg::Point> points);

inline const double EPSILON = 1e-6;
bool IsZero(double value);

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




std::map<string, Color> GetColorForRoute(std::deque<transport_catalogue::Bus> buses, std::vector<Color> color_palette);


deque<string*> GetStopsForNonRounTtip(deque<string*> stops);


void DrawRoute(RequestToTc& rtotc, Render_data& render_data);
void DrawRoute(TransportCatalogue& tc, Render_data& render_data);

void DrawPolyline();

