#pragma once
#define _USE_MATH_DEFINES
#include "svg.h" // Объявления классов библиотеки должны быть расположены в файле svg.h

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
    uint32_t x, y; // x и y — координаты соответствующей остановки;
    geo::Coordinates last_coords; // координаты последней остановки если 1 и посл не равны 
    std::string font = "Verdana"; // Название шрифта
    std::string font_weight = "bold"; // Толщина шрифта
    std::string text_data; // Текстовое содержимое объекта
    std::string* last_stop; // Текстовое содержимое объекта
    Color route_color; 
};


struct Stop_description {
    uint32_t x, y; // x и y — координаты соответствующей остановки;

    uint32_t dx, dy; //смещение dx и dy равно настройке stop_label_offsett;
    uint32_t font_size ; // Размер шрифта size равен настройке stop_label_font_size
    std::string font = "Verdana"; // Название шрифта
    //std::string font_weight = "bold"; // не должно быть 
    std::string stop_name; // назваиние остановки
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
    std::vector<Color> underlayer_color; //поменять на просто Color
    double underlayer_width;
    std::vector<Color> color_palette; //поменять на просто Color

};


svg::Polyline CreatePolyline(vector<svg::Point> points);

inline const double EPSILON = 1e-6;
bool IsZero(double value);

class SphereProjector {
public:
    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
        double max_width, double max_height, double padding)
        : padding_(padding) //
    {
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end) {
            return;
        }

        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            // Коэффициенты масштабирования по ширине и высоте ненулевые,
            // берём минимальный из них
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        }
        else if (width_zoom) {
            // Коэффициент масштабирования по ширине ненулевой, используем его
            zoom_coeff_ = *width_zoom;
        }
        else if (height_zoom) {
            // Коэффициент масштабирования по высоте ненулевой, используем его
            zoom_coeff_ = *height_zoom;
        }
    }

    // Проецирует широту и долготу в координаты внутри SVG-изображения
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

bool  CheckSameStations(deque<string*> current_stops);

// Компаратор для сравнения указателей на строки
struct PointerStringLess {
    bool operator()(const Stop a, const Stop b) const {
        return a.stop_name < b.stop_name;
    }
};

//std::set<std::string> ReturnSetOfstops(std::deque<transport_catalogue::Stop> stops);


class MapRenderer {
public:
    MapRenderer( Render_data& render_data);

    void DrawRoute(TransportCatalogue& tc);

private:
    Render_data& render_data_;
};

