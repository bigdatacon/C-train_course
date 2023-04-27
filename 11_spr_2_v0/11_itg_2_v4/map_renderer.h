#pragma once
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
        Triangle(svg::Point p1, svg::Point p2, svg::Point p3);

        // Реализует метод Draw интерфейса svg::Drawable
        void Draw(svg::ObjectContainer& container) const override;

    private:
        svg::Point p1_, p2_, p3_;
    };

    class Star : public svg::Drawable {
    public:
        Star(svg::Point p1, double  outer_radius, double inner_radius, int num_rays);

        // Реализует метод Draw интерфейса svg::Drawable
        void Draw(svg::ObjectContainer& container) const override;

    private:
        svg::Point p1_;
        double outer_radius_;
        double inner_radius_;
        int num_rays_;
    };

    class Snowman : public svg::Drawable {
    public:
        Snowman(svg::Point head_center, double  head_radius);

        // Реализует метод Draw интерфейса svg::Drawable
        void Draw(svg::ObjectContainer& container) const override;

    private:
        svg::Point head_center_;
        double  head_radius_;

    };

} // namespace shapes

namespace {
    svg::Polyline CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays);
} //namespace


template <typename DrawableIterator>
void DrawPicture(DrawableIterator begin, DrawableIterator end, svg::ObjectContainer& target);

template <typename Container>
void DrawPicture(const Container& container, svg::ObjectContainer& target);

// Выполняет линейную интерполяцию значения от from до to в зависимости от параметра t.
uint8_t Lerp(uint8_t from, uint8_t to, double t);


void TestRender();

