#include "ppm_image.h"

#include <iostream>
#include <string_view>

using namespace std;

void NegateInplace(img_lib::Image& image) {
    int w{ image.GetWidth() };
    int h{ image.GetHeight() };
    
    std::vector<char> buff(w * 3);
    
    for (int i = 0; i < h; ++i) {
        img_lib::Color* line = image.GetLine(i);
        for (int j = 0; j < w; ++j) {
            line[j].r = byte(255 - std::to_integer<int>(line[j].r));
            line[j].g = byte(255 - std::to_integer<int>(line[j].g));
            line[j].b = byte(255 - std::to_integer<int>(line[j].b));
        }
    }
}




int main(int argc, const char** argv) {
    if (argc != 3) {
        cerr << "Usage: "sv << argv[0] << " <input image> <output image>"sv << endl;
        return 1;
    }

    auto image = img_lib::LoadPPM(argv[1]);
    if (!image) {
        cerr << "Error loading image"sv << endl;
        return 2;
    }
    
    NegateInplace(image);

    if (!img_lib::SavePPM(argv[2], image)) {
        cerr << "Error saving image"sv << endl;
        return 3;
    }

    cout << "Image saved successfully!"sv << endl;
}
