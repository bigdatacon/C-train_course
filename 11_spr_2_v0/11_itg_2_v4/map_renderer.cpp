#include "map_renderer.h"


#include <cmath>


using namespace std::literals;
using namespace svg;
using namespace std;


svg::Polyline CreatePolyline(vector<svg::Point> points) {
    using namespace svg;
    Polyline polyline;
    for (auto p : points) { polyline.AddPoint(p); }
    return polyline.SetFillColor("red"s).SetStrokeColor("black"s);
}


bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}


void TestRender() {
    vector<svg::Point> point_to_draw;
    const double WIDTH = 600.0;
    const double HEIGHT = 400.0;
    const double PADDING = 50.0;

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
    doc_polyline.Add(std::move(polyline));
    doc_polyline.Render(cout);



}

