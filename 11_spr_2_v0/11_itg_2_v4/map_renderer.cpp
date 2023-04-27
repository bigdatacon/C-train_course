#include "map_renderer.h"


#include <cmath>


using namespace std::literals;
using namespace svg;
using namespace std;


/*цвет линии stroke определён по правилам выше;
цвет заливки fill должен иметь значение none;
толщина линии stroke-width равна настройке line_width;
Формы конца линии stroke-linecap и соединений stroke-linejoin равны round.*/

svg::Polyline CreatePolyline(vector<svg::Point> points) {
    using namespace svg;
    Polyline polyline;
    for (auto p : points) { polyline.AddPoint(p); }
    //return polyline.SetFillColor("red"s).SetStrokeColor("black"s);
    // рисую линию как по условию для маршрута 
    return polyline;
}


bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

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


/*
void DrawRoute(RequestToTc& rtotc) { 
    vector<svg::Document> docs; 
    Render_data render_data;
    std::vector<std::variant<std::string, std::vector<int>>> color_palette = render_data.color_palette;
    std::deque<transport_catalogue::Bus> buses = rtotc.GetBuses();
    std::deque<transport_catalogue::Stop> stops = rtotc.GetStops();
    std::sort(buses.begin(), buses.end(),
        [](const transport_catalogue::Bus& a, const transport_catalogue::Bus& b) { return a.bus_name < b.bus_name; });
   
    std::map<string, variant<std::string, std::vector<int>>> colors = GetColorForRoute(buses, color_palette);

    for (auto bus : buses) {

        vector<geo::Coordinates> geo_coords;

        variant<std::string, std::vector<int>> current_color;
        deque<string*> current_stops;
        if (colors.count(bus.bus_name)) {
            variant<std::string, std::vector<int>> current_color = colors[bus.bus_name];
        }

        if (bus.type == "true") {
            current_stops = GetStopsForNonRounTtip(bus.stops);
        }
        else { current_stops = bus.stops; }
        for (int i = 0; i < current_stops.size() - 1; i++) {
            const Stop* one = rtotc.FindStop(*current_stops[i]);
            geo_coords.push_back(one->coordinates);
        }
        const SphereProjector proj_one{
            geo_coords.begin(), geo_coords.end(), WIDTH, HEIGHT, PADDING
        };

        vector<svg::Point> point_to_draw;

        for (const auto geo_coord : geo_coords) {
            const svg::Point screen_coord = proj_one(geo_coord);
            cout << '(' << geo_coord.lat << ", "sv << geo_coord.lng << ") -> "sv;
            cout << '(' << screen_coord.x << ", "sv << screen_coord.y << ')' << endl;
            svg::Point p;
            p.x = screen_coord.x;
            p.y = screen_coord.y;
            point_to_draw.push_back(p);
        }

        svg::Document  doc;
        Polyline polyline = CreatePolyline(point_to_draw);

        doc.Add(std::move(polyline));
        docs.push_back(doc);
    }

    for (auto doc : docs) { doc.Render(cout); }

}
*/


void DrawPolyline() {
    Render_data render_data;
    vector<svg::Point> point_to_draw;

    // Точки, подлежащие проецированию
    vector<geo::Coordinates> geo_coords = {
        {43.587795, 39.716901}, {43.581969, 39.719848}, {43.598701, 39.730623},
        {43.585586, 39.733879}, {43.590317, 39.746833}
    };

    // Создаём проектор сферических координат на карту
    const SphereProjector proj{
        geo_coords.begin(), geo_coords.end(), WIDTH, HEIGHT, PADDING
    };

    // Проецируем и выводим координаты
    for (const auto geo_coord : geo_coords) {
        const svg::Point screen_coord = proj(geo_coord);
        //cout << '(' << geo_coord.lat << ", "sv << geo_coord.lng << ") -> "sv;
        //cout << '(' << screen_coord.x << ", "sv << screen_coord.y << ')' << endl;
        svg::Point p;
        p.x = screen_coord.x;
        p.y = screen_coord.y;
        point_to_draw.push_back(p);
    }

    svg::Document  doc_polyline;
    Polyline polyline = CreatePolyline(point_to_draw);

    /*цвет линии stroke определён по правилам выше;
цвет заливки fill должен иметь значение none;
толщина линии stroke-width равна настройке line_width;
Формы конца линии stroke-linecap и соединений stroke-linejoin равны round.*/
    polyline.SetFillColor(NoneColor).SetStrokeColor(render_data.color_palette[0]).SetStrokeWidth(render_data.line_width).SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND);


    doc_polyline.Add(std::move(polyline));
    doc_polyline.Render(cout);

}

