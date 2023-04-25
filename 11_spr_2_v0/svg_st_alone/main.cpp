#define _USE_MATH_DEFINES
#include "svg.h" // Объявления классов библиотеки должны быть расположены в файле svg.h

#include <cmath>


using namespace std::literals;
using namespace svg;
using namespace std;

namespace {
    svg::Polyline CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays);
}

namespace shapes {


    class Triangle : public svg::Drawable {
    public:
        Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
            : p1_(p1)
            , p2_(p2)
            , p3_(p3) {
        }

        // Реализует метод Draw интерфейса svg::Drawable
        void Draw(svg::ObjectContainer& container) const override {
            container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
        }

    private:
        svg::Point p1_, p2_, p3_;
    };

    class Star : public svg::Drawable {
    public:
        Star(svg::Point p1, double  outer_radius, double inner_radius, int num_rays)
            : p1_(p1)
            , outer_radius_(outer_radius)
            , inner_radius_(inner_radius)
            , num_rays_(num_rays) {
        }

        // Реализует метод Draw интерфейса svg::Drawable
        void Draw(svg::ObjectContainer& container) const override {
            //container.AddPtr(std::make_unique<Polyline>(::CreateStar(p1_, outer_radius_, inner_radius_, num_rays_)));
            container.AddPtr(  //
                std::make_unique<Polyline>(
                    ::CreateStar(p1_, outer_radius_, inner_radius_, num_rays_)
                    .SetFillColor("red"s)
                    .SetStrokeColor("black"s)));
        }

    private:
        svg::Point p1_;
        double outer_radius_;
        double inner_radius_;
        int num_rays_;
    };

    class Snowman : public svg::Drawable {
    public:
        Snowman(svg::Point head_center, double  head_radius)
            : head_center_(head_center)
            , head_radius_(head_radius)
        {
        }

        // Реализует метод Draw интерфейса svg::Drawable
        void Draw(svg::ObjectContainer& container) const override {
            container.AddPtr(std::make_unique<Circle>(Circle().SetCenter({ head_center_.x,  head_center_.y + 5 * head_radius_ }).SetRadius(head_radius_ * 2).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"s)));
            container.AddPtr(std::make_unique<Circle>(Circle().SetCenter({ head_center_.x, head_center_.y + 2 * head_radius_ }).SetRadius(head_radius_ * 1.5).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"s)));
            container.AddPtr(std::make_unique<Circle>(Circle().SetCenter({ head_center_.x, head_center_.y }).SetRadius(head_radius_).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"s)));

        }

    private:
        svg::Point head_center_;
        double  head_radius_;

    };

} // namespace shapes

namespace {
    svg::Polyline CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays) {
        using namespace svg;
        Polyline polyline;
        for (int i = 0; i <= num_rays; ++i) {
            double angle = 2 * M_PI * (i % num_rays) / num_rays;
            polyline.AddPoint({ center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle) });
            if (i == num_rays) {
                break;
            }
            angle += M_PI / num_rays;
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

/*int main() {

// Задаёт цвет в виде четырёх компонент: red, green, blue, opacity
svg::Rgba rgba{100, 20, 50, 0.3};
assert(rgba.red == 100);
assert(rgba.green == 20);
assert(rgba.blue == 50);
assert(rgba.opacity == 0.3);

// Чёрный непрозрачный цвет: red=0, green=0, blue=0, alpha=1.0
svg::Rgba color;
assert(color.red == 0 && color.green == 0 && color.blue == 0 && color.opacity == 1.0);
    using namespace svg;
    using namespace std;

    const uint8_t start_r = 0;
    const uint8_t end_r = 20;
    const uint8_t start_g = 255;
    const uint8_t end_g = 20;
    const uint8_t start_b = 30;
    const uint8_t end_b = 150;

    const int num_circles = 10;
    Document doc;
    for (int i = 0; i < num_circles; ++i) {
        const double t = double(i) / (num_circles - 1);

        const string r = to_string(Lerp(start_r, end_r, t));
        const string g = to_string(Lerp(start_g, end_g, t));
        const string b = to_string(Lerp(start_b, end_b, t));

        string fill_color = "rgb("s + r + ","s + g + ","s + b + ")"s;
        doc.Add(Circle()
                    .SetFillColor(fill_color)
                    .SetStrokeColor("black"s)
                    .SetCenter({i * 20.0 + 40, 40.0})
                    .SetRadius(15));
    }
    doc.Render(cout);
} */

int main() {
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

    Document doc;
    doc.Add(std::move(c));
    doc.Render(cout);
}

