#define _USE_MATH_DEFINES
#include "svg.h" // ���������� ������� ���������� ������ ���� ����������� � ����� svg.h

#include <cmath>


using namespace std::literals;
using namespace svg;
using namespace std;

namespace shapes {

    class Triangle : public svg::Drawable {
    public:
        Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
            : p1_(p1)
            , p2_(p2)
            , p3_(p3) {
        }

        // ��������� ����� Draw ���������� svg::Drawable
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

        // ��������� ����� Draw ���������� svg::Drawable
        void Draw(svg::ObjectContainer& container) const override {
            container.AddPtr(std::make_unique<Polyline>(::CreateStar(p1_, outer_radius_, inner_radius_, num_rays_)));
        }

    private:
        svg::Point p1_;
        double outer_radius_;
        double inner_radius_;
        int num_rays_;
    };

    class Snowman : public svg::Drawable {
        Snowman(svg::Point head_center, double  head_radius)
            : head_center_(head_center)
            , head_radius_(head_radius)
        {
        }

        // ��������� ����� Draw ���������� svg::Drawable
        void Draw(svg::ObjectContainer& container) const override {
            container.AddPtr(std::make_unique<Circle>(head_center_, head_radius_));
            container.AddPtr(std::make_unique<Circle>(Point(head_center_.x, head_center_.y + 2 * head_radius_), 1.5 * head_radius_));
            container.AddPtr(std::make_unique<Circle>(Point(head_center_.x, head_center_.y + 5 * head_radius_), 2 * head_radius_));
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

int main() {
    using namespace svg;
    using namespace shapes;
    using namespace std;

    vector<unique_ptr<svg::Drawable>> picture;

    picture.emplace_back(make_unique<Triangle>(Point{ 100, 20 }, Point{ 120, 50 }, Point{ 80, 40 }));
    // 5-������� ������ � ������� {50, 20}, ������ ����� 10 � ���������� �������� 4
    picture.emplace_back(make_unique<Star>(Point{ 50.0, 20.0 }, 10.0, 4.0, 5));
    // �������� � "�������" �������� 10, ������� ����� � ����� {30, 20}
    picture.emplace_back(make_unique<Snowman>(Point{ 30, 20 }, 10.0));

    svg::Document doc;
    // ��� ��� �������� ��������� ��������� ObjectContainer,
    // ��� ����� �������� � DrawPicture � �������� ���� ��� ���������
    DrawPicture(picture, doc);

    // ������� ���������� �������� � stdout
    doc.Render(cout);
}