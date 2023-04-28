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

std::map<string, Color> GetColorForRoute(std::deque<transport_catalogue::Bus> buses, std::vector<Color> color_palette) {
    std::map<string, Color> result;
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

    return reverse_stops;

}



vector<geo::Coordinates> GetAllCoordinates(TransportCatalogue& rtotc, std::deque<transport_catalogue::Bus> buses) {
    vector<geo::Coordinates> geo_coords;
    for (auto bus : buses) {
        deque<string*> current_stops;
        if (bus.type != "true") {
            current_stops = GetStopsForNonRounTtip(bus.stops);
        }
        else { current_stops = bus.stops; }
        for (int i = 0; i < current_stops.size(); i++) {
            const Stop* one = rtotc.FindStop(*current_stops[i]);
            geo_coords.push_back(one->coordinates);
        }
    }
    return geo_coords;
}
////////////////////////////////START\\\\\\\\\\\\\\\\\\\\\\\\\\
    
MapRenderer::MapRenderer( Render_data& render_data)
:render_data_(render_data)
{
    // Ваш код для инициализации объектов
}

svg::Document MapRenderer::DrawRoute(TransportCatalogue& tc) {

    vector<svg::Document> docs;
    vector<svg::Polyline> routes_vec;
    std::vector<Color> color_palette = render_data_.color_palette;
    std::deque<transport_catalogue::Bus> buses = tc.GetBuses();
    std::deque<transport_catalogue::Stop> stops = tc.GetStops();
    std::sort(buses.begin(), buses.end(),
        [](const transport_catalogue::Bus& a, const transport_catalogue::Bus& b) { return a.bus_name < b.bus_name; });

    std::map<string, Color> colors = GetColorForRoute(buses, color_palette);
    vector<geo::Coordinates> geo_coords = GetAllCoordinates(tc, buses);

    const SphereProjector proj_one{
    geo_coords.begin(), geo_coords.end(), render_data_.width, render_data_.height, render_data_.padding
    };

    for (auto bus : buses) {
        Color current_color;
        deque<string*> current_stops;
        if (colors.count(bus.bus_name)) {
            {
                current_color = colors[bus.bus_name];
            }

        }
        vector<svg::Point> point_to_draw;

        if (bus.type != "true") {
            current_stops = GetStopsForNonRounTtip(bus.stops);
        }
        else { current_stops = bus.stops; }
        for (int i = 0; i < current_stops.size(); i++) {
            const Stop* one = tc.FindStop(*current_stops[i]);
            const svg::Point screen_coord = proj_one(one->coordinates);
            svg::Point p;
            p.x = screen_coord.x;
            p.y = screen_coord.y;
            point_to_draw.push_back(p);
        }

        svg::Document  doc;
        Polyline polyline = CreatePolyline(point_to_draw);
        polyline.SetFillColor(NoneColor).SetStrokeColor(current_color).SetStrokeWidth(render_data_.line_width).SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND);

        routes_vec.push_back(polyline);

    }
    svg::Document  doc;
    for (auto polyline : routes_vec) {
        doc.Add(std::move(polyline));
    }
    //doc.Render(cout);
    return doc;

}






//////////////////////////////////////////////END
/*
void DrawRoute(TransportCatalogue& rtotc, Render_data& render_data) {

    vector<svg::Document> docs;
    vector<svg::Polyline> routes_vec;
    std::vector<Color> color_palette = render_data.color_palette;
    std::deque<transport_catalogue::Bus> buses = rtotc.GetBuses();
    std::deque<transport_catalogue::Stop> stops = rtotc.GetStops();
    std::sort(buses.begin(), buses.end(),
        [](const transport_catalogue::Bus& a, const transport_catalogue::Bus& b) { return a.bus_name < b.bus_name; });

    std::map<string, Color> colors = GetColorForRoute(buses, color_palette);
    vector<geo::Coordinates> geo_coords = GetAllCoordinates(rtotc,  buses);

    const SphereProjector proj_one{
    geo_coords.begin(), geo_coords.end(), render_data.width, render_data.height, render_data.padding
    };

    for (auto bus : buses) {
        Color current_color;
        deque<string*> current_stops;
        if (colors.count(bus.bus_name)) {
            { 
                current_color = colors[bus.bus_name];
            }
            
        }

        vector<svg::Point> point_to_draw;

        if (bus.type != "true") {
            current_stops = GetStopsForNonRounTtip(bus.stops);
        }
        else { current_stops = bus.stops; }
        for (int i = 0; i < current_stops.size(); i++) {
            const Stop* one = rtotc.FindStop(*current_stops[i]);
            const svg::Point screen_coord = proj_one(one->coordinates);
            svg::Point p;
            p.x = screen_coord.x;
            p.y = screen_coord.y;
            point_to_draw.push_back(p);
        }


        svg::Document  doc;
        Polyline polyline = CreatePolyline(point_to_draw);
        polyline.SetFillColor(NoneColor).SetStrokeColor(current_color).SetStrokeWidth(render_data.line_width).SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND);

        routes_vec.push_back(polyline);

    }
    svg::Document  doc;
    for (auto polyline : routes_vec) {
        
        doc.Add(std::move(polyline));

    }
    doc.Render(cout);

}


*/