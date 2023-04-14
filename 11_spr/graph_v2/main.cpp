#define _USE_MATH_DEFINES
#include "svg.h" // Объявления классов библиотеки должны быть расположены в файле svg.h
 
#include <cmath>
 
using namespace std::literals;
using namespace svg;
using namespace std;
void TestText() {
        {
            Text().Render(cout);
            cout << "endl";
        } {
            Text().SetOffset({0, 6}).SetFontSize(12).Render(cout);
            cout << "endl";
        } {
            Text().SetPosition({35, 20})
                    .SetFontFamily("Verdana"s)
                    .SetFontSize(12)
                    .SetOffset({0, 6})
                    .SetFontWeight("bold"s)
                    .SetData("Hello C++"s)
                .Render(cout);
            cout << "endl";
        } {
            Text().SetFontFamily("Verdana"s)
                    .SetPosition({35, 20})
                    .SetOffset({0, 6})
                    .SetFontSize(12)
                    .SetFontWeight("bold"s)
                    .SetData("Hello C++"s)
                .Render(cout);
        }
    }
int main(){
    TestText();
}
