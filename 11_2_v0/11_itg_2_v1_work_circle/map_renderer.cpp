#include "map_renderer.h"


#include <chrono>
#include <cmath>
#include <algorithm>


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
        std::deque<std::string*> current_stops;
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


bool  CheckSameStations(deque<string*> current_stops) {
    if (*current_stops.front() == *current_stops.back()) {
        return true;
    }
    else {
        return false;
    }
}

// Компаратор для сравнения указателей на строки
struct PointerStringLess {
    bool operator()(const Stop a, const Stop b) const {
        return a.stop_name < b.stop_name;
    }
};

std::set<std::string> ReturnSetOfstops(std::deque<transport_catalogue::Stop> stops) {
    std::set<std::string> res;
    for (auto el : stops) { res.insert(el.stop_name); }
    return res;

}

MapRenderer::MapRenderer(Render_data & render_data)
    :render_data_(render_data)
{
    // Ваш код для инициализации объектов
}

void MapRenderer::DrawRoute(TransportCatalogue& tc) {

    vector<Text> stops_names;
    vector<Circle> stops_circles;
    vector<svg::Document> docs;
    vector<svg::Polyline> routes_vec;
    vector<svg::Text> routes_text;
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


    std::set<std::string> stops_for_route = ReturnSetOfstops(stops);

    for (auto bus : buses) {
        if (bus.stops.size() == 0) { return; }
        cout << " bus : " << bus.bus_name << endl;
        cout << "bus stops : " << endl;
        for (auto el : bus.stops) { cout << *el << endl; }



        bool same_stations = true;
        Rout_description rout_description;
        rout_description.text_data = bus.bus_name;
        vector<std::pair<string*, svg::Point>> route_stations;
        

        Color current_color;
        deque<string*> current_stops;
        if (colors.count(bus.bus_name)) {
            {
                current_color = colors[bus.bus_name];
            }

        }
        rout_description.route_color = current_color;
        rout_description.last_stop = bus.stops.back();

        vector<svg::Point> point_to_draw;
        

        if (bus.type != "true") {
            current_stops = GetStopsForNonRounTtip(bus.stops);
            same_stations = CheckSameStations(bus.stops);
        }
        else { current_stops = bus.stops;
        
        }


        for (int i = 0; i < current_stops.size(); i++) {
            const Stop* one = tc.FindStop(*current_stops[i]);
            const svg::Point screen_coord = proj_one(one->coordinates);
            svg::Point p;
            p.x = screen_coord.x;
            p.y = screen_coord.y;

            if (i == 0) {
                Text route_font;
                Text route;

                route_font.SetPosition(Point(screen_coord.x, screen_coord.y)).SetOffset(Point(render_data_.bus_label_offset[0], render_data_.bus_label_offset[1])).SetFontSize(render_data_.bus_label_font_size)\
                    .SetFontFamily(rout_description.font).SetFontWeight(rout_description.font_weight).SetData(rout_description.text_data)\
                    .SetFillColor(render_data_.underlayer_color[0]).SetStrokeColor(render_data_.underlayer_color[0]).SetStrokeWidth(render_data_.underlayer_width).SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND);

                route.SetPosition(Point(screen_coord.x, screen_coord.y)).SetOffset(Point(render_data_.bus_label_offset[0], render_data_.bus_label_offset[1])).SetFontSize(render_data_.bus_label_font_size)\
                    .SetFontFamily(rout_description.font).SetFontWeight(rout_description.font_weight).SetData(rout_description.text_data).SetFillColor(rout_description.route_color);

                routes_text.push_back(route_font);
                routes_text.push_back(route);
            }
            
            point_to_draw.push_back(p);
        }

        if (!same_stations) {
            const Stop* last = tc.FindStop(*rout_description.last_stop);
            const svg::Point screen_coord_last = proj_one(last->coordinates);
            Text route_font_not_same;
            Text route_not_same;

            route_font_not_same.SetPosition(Point(screen_coord_last.x, screen_coord_last.y)).SetOffset(Point(render_data_.bus_label_offset[0], render_data_.bus_label_offset[1])).SetFontSize(render_data_.bus_label_font_size)\
                .SetFontFamily(rout_description.font).SetFontWeight(rout_description.font_weight).SetData(rout_description.text_data)\
                .SetFillColor(render_data_.underlayer_color[0]).SetStrokeColor(render_data_.underlayer_color[0]).SetStrokeWidth(render_data_.underlayer_width).SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND);

            route_not_same.SetPosition(Point(screen_coord_last.x, screen_coord_last.y)).SetOffset(Point(render_data_.bus_label_offset[0], render_data_.bus_label_offset[1])).SetFontSize(render_data_.bus_label_font_size)\
                .SetFontFamily(rout_description.font).SetFontWeight(rout_description.font_weight).SetData(rout_description.text_data).SetFillColor(rout_description.route_color);

            routes_text.push_back(route_font_not_same);
            routes_text.push_back(route_not_same);



        }

        svg::Document  doc;
        Polyline polyline = CreatePolyline(point_to_draw);
        polyline.SetFillColor(NoneColor).SetStrokeColor(current_color).SetStrokeWidth(render_data_.line_width).SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND);

        routes_vec.push_back(polyline);


    }


    // обработка остановок и текста к ним 
    for (auto i = stops_for_route.begin(); i != stops_for_route.end(); ++i)
    {

        Circle c;
        const Stop* one = tc.FindStop(*i);
        const svg::Point screen_coord = proj_one(one->coordinates);


        c.SetRadius(render_data_.stop_radius).SetCenter({ screen_coord.x, screen_coord.y });
        c.SetFillColor("white");
        stops_circles.push_back(c);

        cout << "ADD : " << *i << " : " << screen_coord.x << " : " << screen_coord.y << endl;


        Text stop_description_font;
        Text stop_description;

        stop_description_font.SetPosition(Point(screen_coord.x, screen_coord.y)).SetOffset(Point(render_data_.stop_label_offset[0], render_data_.stop_label_offset[1])).SetFontSize(render_data_.stop_label_font_size)\
            .SetFontFamily("Verdana").SetData(*i)\
            .SetFillColor(render_data_.underlayer_color[0]).SetStrokeColor(render_data_.underlayer_color[0]).SetStrokeWidth(render_data_.underlayer_width).SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND);


        stop_description.SetPosition(Point(screen_coord.x, screen_coord.y)).SetOffset(Point(render_data_.stop_label_offset[0], render_data_.stop_label_offset[1])).SetFontSize(render_data_.stop_label_font_size)\
            .SetFontFamily("Verdana").SetData(*i).SetFillColor("black");

        stops_names.push_back(stop_description_font);
        stops_names.push_back(stop_description);

    }


    svg::Document  doc;
    for (auto polyline : routes_vec) {
        doc.Add(std::move(polyline));
    }

    for (auto text : routes_text) {
        doc.Add(std::move(text));
    }
    
    cout << "stops_circles.size() : " << stops_circles.size()  << endl;

    for (auto c : stops_circles){
        doc.Add(std::move(c));
    }
    /*
    for (auto stop : stops_names) {
        doc.Add(std::move(stop));
    }
    */
  

    doc.Render(cout);
    
}

/*

void MapRenderer::DrawRoute(TransportCatalogue& tc) {

    auto start_DrawRoute = std::chrono::high_resolution_clock::now(); // начальное время




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
    doc.Render(cout);
    //return doc;
    auto end_time_start_DrawRoute = std::chrono::high_resolution_clock::now(); // конечное время
    auto duration_start_DrawRoute = std::chrono::duration_cast<std::chrono::microseconds>(end_time_start_DrawRoute - start_DrawRoute); // время выполнения в микросекундах
    std::cout << "TIME PASSED DrawRoute : " << duration_start_DrawRoute.count() << " MILISECOND" << std::endl;
}

*/






