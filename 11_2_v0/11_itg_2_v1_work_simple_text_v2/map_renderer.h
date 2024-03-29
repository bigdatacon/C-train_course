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

struct Rout_description {
    uint32_t x, y; // x � y � ���������� ��������������� ���������;
    uint32_t x_last, y_last; // ���������� ��������� ��������� ���� 1 � ���� �� ����� 
    //uint32_t dx, dy; //�������� dx � dy ����� ��������� bus_label_offset;
    //uint32_t font_size ; // ������ ������ size ����� ��������� bus_label_font_size;
    std::string font = "Verdana"; // �������� ������
    std::string font_weight = "bold"; // ������� ������
    std::string text_data; // ��������� ���������� �������
    Color route_color; 
};


struct Stop_description {
    uint32_t x, y; // x � y � ���������� ��������������� ���������;

    uint32_t dx, dy; //�������� dx � dy ����� ��������� stop_label_offsett;
    uint32_t font_size ; // ������ ������ size ����� ��������� stop_label_font_size
    std::string font = "Verdana"; // �������� ������
    //std::string font_weight = "bold"; // �� ������ ���� 
    std::string stop_name; // ��������� ���������
    std::string stop_color="black";
    

};


struct Render_data {
    //Render_data() = default;
    double width;
    double height;
    double padding;
    double line_width;
    //int line_width;
    double stop_radius;
    int bus_label_font_size;
    std::vector<double> bus_label_offset;
    int stop_label_font_size;
    std::vector<double> stop_label_offset;
    //Rgba underlayer_color;
    std::vector<Color> underlayer_color; //�������� �� ������ Color
    double underlayer_width;
    std::vector<Color> color_palette; //�������� �� ������ Color

};


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


vector<geo::Coordinates> GetAllCoordinates(TransportCatalogue& rtotc, std::deque<transport_catalogue::Bus> buses);

std::map<string, Color> GetColorForRoute(std::deque<transport_catalogue::Bus> buses, std::vector<Color> color_palette);
deque<string*> GetStopsForNonRounTtip(deque<string*> stops);

void DrawRoute(TransportCatalogue& rtotc, Render_data& render_data);


class MapRenderer {
public:
    MapRenderer( Render_data& render_data);

    void DrawRoute(TransportCatalogue& tc);

private:
    Render_data& render_data_;
};


