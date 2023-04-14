#define _USE_MATH_DEFINES
#include "svg.h"

#include <cmath>

using namespace std::literals;
using namespace svg;

/*
������ ������������� ����������. �� ����� ��������������� � ��������, ����� �� ����������
��� ������ ����������.
*/
/*
namespace {

Polyline CreateStar(Point center, double outer_rad, double inner_rad, int num_rays) {
    Polyline polyline;
    for (int i = 0; i <= num_rays; ++i) {
        double angle = 2 * M_PI * (i % num_rays) / num_rays;
        polyline.AddPoint({center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle)});
        if (i == num_rays) {
            break;
        }
        angle += M_PI / num_rays;
        polyline.AddPoint({center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle)});
    }
    return polyline;
}

// ������� �����������, ���� � ������
void DrawPicture() {
    Document doc;
    doc.Add(Circle().SetCenter({20, 20}).SetRadius(10));
    doc.Add(Text()
                .SetFontFamily("Verdana"s)
                .SetPosition({35, 20})
                .SetOffset({0, 6})
                .SetFontSize(12)
                .SetFontWeight("bold"s)
                .SetData("Hello C++"s));
    doc.Add(CreateStar({20, 50}, 10, 5, 5));
    doc.Render(std::cout);
}

}  // namespace
*/

int main() {
    /*
       ��� ������ ��� ����������� ������ ������ Circle, ������� � ��������� �������.
       ����� ���� ��� �� ���������� ���������� ����� Document, ������������ ����������
       ����� ����� ������� ���:

       Document doc;
       doc.Add(Circle().SetCenter({20, 20}).SetRadius(10));
       doc.Render(std::cout);
    */
    std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
    std::cout << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;

    Circle c;
    c.SetCenter({ 20, 20 }).SetRadius(10);
    RenderContext ctx(std::cout, 2, 2);
    c.Render(ctx);

    std::cout << "</svg>"sv;
}