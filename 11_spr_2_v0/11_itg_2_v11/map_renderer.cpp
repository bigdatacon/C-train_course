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



void DrawRoute(RequestToTc& rtotc, Render_data& render_data) {
    const double WIDTH = 600.0;
    const double HEIGHT = 400.0;
    const double PADDING = 50.0;
    vector<svg::Document> docs;
    vector<svg::Polyline> routes_vec;
    //Render_data render_data = inprjs.GetRenderData();
    std::vector<Color> color_palette = render_data.color_palette;
    std::deque<transport_catalogue::Bus> buses = rtotc.GetBuses();
    std::deque<transport_catalogue::Stop> stops = rtotc.GetStops();
    std::sort(buses.begin(), buses.end(),
        [](const transport_catalogue::Bus& a, const transport_catalogue::Bus& b) { return a.bus_name < b.bus_name; });

    std::map<string, Color> colors = GetColorForRoute(buses, color_palette);

    for (auto bus : buses) {

        vector<geo::Coordinates> geo_coords;

        variant<std::string, std::vector<int>> current_color;
        deque<string*> current_stops;
        if (colors.count(bus.bus_name)) {
            svg::Color current_color = colors[bus.bus_name];
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

        //doc.Add(std::move(polyline));
        routes_vec.push_back(polyline);

        //docs.push_back(doc);
        //docs.push_back(std::move(doc));
    }

    for (auto polyline : routes_vec) {
        svg::Document  doc;
        doc.Add(std::move(polyline));
        doc.Render(cout);
    }

}

// под класс каталога, так как через дочерний класс не работает 
void DrawRoute(TransportCatalogue& rtotc, Render_data& render_data) {

    vector<svg::Document> docs;
    vector<svg::Polyline> routes_vec;
    //Render_data render_data = inprjs.GetRenderData();
    std::vector<Color> color_palette = render_data.color_palette;
    std::deque<transport_catalogue::Bus> buses = rtotc.GetBuses();
    std::deque<transport_catalogue::Stop> stops = rtotc.GetStops();
    std::sort(buses.begin(), buses.end(),
        [](const transport_catalogue::Bus& a, const transport_catalogue::Bus& b) { return a.bus_name < b.bus_name; });

    std::map<string, Color> colors = GetColorForRoute(buses, color_palette);

    for (auto bus : buses) {

        vector<geo::Coordinates> geo_coords;

        Color current_color;
        deque<string*> current_stops;
        if (colors.count(bus.bus_name)) {
            { 
                //cout << "EST BUS : " << bus.bus_name << endl;  
                current_color = colors[bus.bus_name];
            //cout << "current_color: " << current_color << endl;
            }
            
        }

        if (bus.type != "true") {
            current_stops = GetStopsForNonRounTtip(bus.stops);
        }
        else { current_stops = bus.stops; }
        for (int i = 0; i < current_stops.size() ; i++) {
            const Stop* one = rtotc.FindStop(*current_stops[i]);
            geo_coords.push_back(one->coordinates);
        }

        //cout << "bus.bus_name : " << bus.bus_name << endl;
        //cout << "coords :" << endl;
        /*for (auto el : geo_coords) {
            cout << el.lat << " : " << el.lng << endl;
        }
        cout << "bus.stops : " << endl;
        for (auto el : bus.stops) { cout << *el << endl; }
        cout << "current_stops : " << endl;
        for (auto el : current_stops) { cout << *el << endl; }*/


        const SphereProjector proj_one{
            geo_coords.begin(), geo_coords.end(), render_data.width, render_data.height, render_data.padding
        };

        vector<svg::Point> point_to_draw;

        for (const auto geo_coord : geo_coords) {
            const svg::Point screen_coord = proj_one(geo_coord);

            svg::Point p;
            p.x = screen_coord.x;
            p.y = screen_coord.y;
            point_to_draw.push_back(p);
        }

        svg::Document  doc;
        Polyline polyline = CreatePolyline(point_to_draw);
        //cout << "current_color before insert : " << current_color << endl;
        polyline.SetFillColor(NoneColor).SetStrokeColor(current_color).SetStrokeWidth(render_data.line_width).SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND);


        //doc.Add(std::move(polyline));
        routes_vec.push_back(polyline);

        //docs.push_back(doc);
        //docs.push_back(std::move(doc));
    }
    svg::Document  doc;
    for (auto polyline : routes_vec) {
        
        doc.Add(std::move(polyline));

    }
    doc.Render(cout);

}



void DrawPolyline() {

    const double WIDTH = 600.0;
    const double HEIGHT = 400.0;
    const double PADDING = 50.0;

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
    polyline.SetFillColor(NoneColor)/*.SetStrokeColor(render_data.color_palette[0])*/.SetStrokeWidth(render_data.line_width).SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND);


    doc_polyline.Add(std::move(polyline));
    doc_polyline.Render(cout);

}

