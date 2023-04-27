#include "map_renderer.h"


#include <cmath>


using namespace std::literals;
using namespace svg;
using namespace std;

namespace {
    svg::Polyline CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays);
}

namespace shapes {
    Triangle::Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
        : p1_(p1)
        , p2_(p2)
        , p3_(p3) {
    }


        // Реализует метод Draw интерфейса svg::Drawable
        void Triangle::Draw(svg::ObjectContainer& container) const  {
            container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
        };

        Star::Star(svg::Point p1, double  outer_radius, double inner_radius, int num_rays)
            : p1_(p1)
            , outer_radius_(outer_radius)
            , inner_radius_(inner_radius)
            , num_rays_(num_rays) {
        }

        // Реализует метод Draw интерфейса svg::Drawable
        void Star::Draw(svg::ObjectContainer& container) const  {
            //container.AddPtr(std::make_unique<Polyline>(::CreateStar(p1_, outer_radius_, inner_radius_, num_rays_)));
            container.AddPtr(  //
                std::make_unique<Polyline>(
                    ::CreateStar(p1_, outer_radius_, inner_radius_, num_rays_)
                    .SetFillColor("red"s)
                    .SetStrokeColor("black"s)));
        }


        Snowman::Snowman(svg::Point head_center, double  head_radius)
            : head_center_(head_center)
            , head_radius_(head_radius)
        {
        }

        // Реализует метод Draw интерфейса svg::Drawable
        void Snowman::Draw(svg::ObjectContainer& container) const {
            container.AddPtr(std::make_unique<Circle>(Circle().SetCenter({ head_center_.x,  head_center_.y + 5 * head_radius_ }).SetRadius(head_radius_ * 2).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"s)));
            container.AddPtr(std::make_unique<Circle>(Circle().SetCenter({ head_center_.x, head_center_.y + 2 * head_radius_ }).SetRadius(head_radius_ * 1.5).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"s)));
            container.AddPtr(std::make_unique<Circle>(Circle().SetCenter({ head_center_.x, head_center_.y }).SetRadius(head_radius_).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"s)));

        }



} // namespace shapes

namespace {
    svg::Polyline CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays) {
        using namespace svg;
        Polyline polyline;
        for (int i = 0; i <= num_rays; ++i) {
            double angle = 2 * 3.14 * (i % num_rays) / num_rays;
            polyline.AddPoint({ center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle) });
            if (i == num_rays) {
                break;
            }
            angle += 3.14 / num_rays;
            polyline.AddPoint({ center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle) });
        }
        return polyline;
    }
} //namespace


template <typename DrawableIterator>
void DrawPicture(DrawableIterator begin, DrawableIterator end, svg::ObjectContainer& target) {
    for (auto it = begin; it != end; ++it) {
        (*it)->Draw(target);
    }
}

template <typename Container>
void DrawPicture(const Container& container, svg::ObjectContainer& target) {
    using namespace std;
    DrawPicture(begin(container), end(container), target);
}

// Выполняет линейную интерполяцию значения от from до to в зависимости от параметра t.
uint8_t Lerp(uint8_t from, uint8_t to, double t) {
    return static_cast<uint8_t>(std::round((to - from) * t + from));
}


bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}


void TestRender() {
    using namespace svg;
    using namespace std;

    Color none_color;
    cout << none_color << endl; // none

    Color purple{ "purple"s };
    cout << purple << endl; // purple

    Color rgb = Rgb{ 100, 200, 255 };
    cout << rgb << endl; // rgb(100,200,255)

    Color rgba = Rgba{ 100, 200, 255, 0.5 };
    cout << rgba << endl; // rgba(100,200,255,0.5)

    Circle c;
    c.SetRadius(3.5).SetCenter({ 1.0, 2.0 });
    c.SetFillColor(rgba);
    c.SetStrokeColor(purple);

    svg::Document doc;
    doc.Add(std::move(c));
    doc.Render(cout);

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
        cout << '(' << geo_coord.lat << ", "sv << geo_coord.lng << ") -> "sv;
        cout << '(' << screen_coord.x << ", "sv << screen_coord.y << ')' << endl;
    }





}

