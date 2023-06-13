-----------------------------------------------------------------------
CMakeLists.txt
-----------------------------------------------------------------------
cmake_minimum_required(VERSION 3.11)
 
project(Negate CXX)
set(CMAKE_CXX_STANDARD 17)
 
set(IMGLIB_FILES img_lib.h img_lib.cpp
                 ppm_image.h ppm_image.cpp)
 
if(CMAKE_SYSTEM_NAME MATCHES "^MINGW")
    set(SYSTEM_LIBS -lstdc++)
else()
    set(SYSTEM_LIBS) 
endif()
 
add_executable("hmirror" "hmirr.cpp" ${IMGLIB_FILES})
add_executable("vmirror" "vmirr.cpp" ${IMGLIB_FILES})
add_executable("sobel" "sobel.cpp" ${IMGLIB_FILES})
-----------------------------------------------------------------------
hmirr.cpp
-----------------------------------------------------------------------
#include "ppm_image.h"
 
#include <iostream>
#include <string_view>
 
using namespace std;
 
void HMirrInplace(img_lib::Image& image) {
    
    int height = image.GetHeight();
    int middle = image.GetWidth()/2;
    
    for (int i = 0; i < height; ++i) {
        
        for (int j = 0; j < middle; ++j) {
            
            std::swap(image.GetPixel(j, i).r, image.GetPixel(image.GetWidth() - j - 1, i).r);
            std::swap(image.GetPixel(j, i).g, image.GetPixel(image.GetWidth() - j - 1, i).g);
            std::swap(image.GetPixel(j, i).b, image.GetPixel(image.GetWidth() - j - 1, i).b);
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
 
    HMirrInplace(image);
 
    if (!img_lib::SavePPM(argv[2], image)) {
        cerr << "Error saving image"sv << endl;
        return 3;
    }
 
    cout << "Image saved successfully!"sv << endl;
}
-----------------------------------------------------------------------
vmirr.cpp
-----------------------------------------------------------------------
#include "ppm_image.h"
 
#include <algorithm>
#include <iostream>
#include <string_view>
 
using namespace std;
 
void VMirrInplace(img_lib::Image& image) {
    
    int width = image.GetWidth();
    int middle = image.GetHeight()/2;
    
    for (int i = 0; i < middle; ++i) {
        
        for (int j = 0; j < width; ++j) {
            
            std::swap(image.GetPixel(j, i).r, image.GetPixel(j, image.GetHeight() - i - 1).r);
            std::swap(image.GetPixel(j, i).g, image.GetPixel(j, image.GetHeight() - i - 1).g);
            std::swap(image.GetPixel(j, i).b, image.GetPixel(j, image.GetHeight() - i - 1).b);
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
 
    VMirrInplace(image);
 
    if (!img_lib::SavePPM(argv[2], image)) {
        cerr << "Error saving image"sv << endl;
        return 3;
    }
 
    cout << "Image saved successfully!"sv << endl;
}
-----------------------------------------------------------------------
sobel.cpp
-----------------------------------------------------------------------
#include "ppm_image.h"
 
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <string_view>
 
using namespace std;
 
int Sum(img_lib::Color c) {return to_integer<int>(c.r) + to_integer<int>(c.g) + to_integer<int>(c.b);}
 
img_lib::Image Sobel(const img_lib::Image& image) {
    
    img_lib::Color color = img_lib::Color::Black();
    int width = image.GetWidth() - 1;
    int height = image.GetHeight() - 1;
    
    img_lib::Image result(width + 1, height + 1, color);
    
    for (int i = 1; i < height; ++i) {
        
        for (int j = 1; j < width; ++j) {
            
            int tl = Sum(image.GetPixel(j - 1, i - 1));
            int tc = Sum(image.GetPixel(j, i - 1));
            int tr = Sum(image.GetPixel(j + 1, i - 1));
            
            int cl = Sum(image.GetPixel(j - 1, i));
            int cr = Sum(image.GetPixel(j + 1, i));
 
            int bl = Sum(image.GetPixel(j - 1, i + 1));
            int bc = Sum(image.GetPixel(j, i + 1));
            int br = Sum(image.GetPixel(j + 1, i + 1));
 
            int gx = -tl - 2 * tc - tr + bl + 2 * bc + br;
            int gy = -tl - 2 * cl - bl + tr + 2 * cr + br;
 
            byte helper = byte(std::clamp<double>(std::sqrt(gx * gx + gy * gy), 0, 255));
 
            result.GetPixel(j, i).r = helper;
            result.GetPixel(j, i).g = helper;
            result.GetPixel(j, i).b = helper;
        }
    }
 
    return result;
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
 
    image = Sobel(image);
 
    if (!img_lib::SavePPM(argv[2], image)) {
        cerr << "Error saving image"sv << endl;
        return 3;
    }
 
    cout << "Image saved successfully!"sv << endl;
}
-----------------------------------------------------------------------
img_lib.h
-----------------------------------------------------------------------
#pragma once
 
#include <array>
#include <cassert>
#include <cstddef>
#include <vector>
 
namespace img_lib {
 
struct Size {
    int width;
    int height;
};
 
struct Color {
    
    static Color Black() {
        
        return {std::byte{0}, 
                std::byte{0}, 
                std::byte{0}, 
                std::byte{255}};
    }
 
    std::byte r, g, b, a;
};
 
class Image {
public:
 
    Image() = default;
 
    Image(int w, int h, Color fill);
 
    Color GetPixel(int x, int y) const {return const_cast<Image*>(this)->GetPixel(x, y);}
    
    Color& GetPixel(int x, int y) {
        assert(x < GetWidth() && y < GetHeight() && x >= 0 && y >= 0);
        return GetLine(y)[x];
    }
 
    Color* GetLine(int y);
    const Color* GetLine(int y) const;
 
    int GetWidth() const;
    int GetHeight() const;
 
    int GetStep() const;
 
    explicit operator bool() const {return GetWidth() > 0 && GetHeight() > 0;}
    bool operator!() const {return !operator bool();}
 
private:
    int width_ = 0;
    int height_ = 0;
    int step_;
 
    std::vector<Color> pixels_;
};
 
}//end namespace img_lib
-----------------------------------------------------------------------
img_lib.cpp
-----------------------------------------------------------------------
#include "img_lib.h"
 
namespace img_lib {
 
Image::Image(int w, int h, Color fill) : width_(w)
                                       , height_(h)
                                       , step_(w)
                                       , pixels_(step_ * height_, fill) {}
 
Color* Image::GetLine(int y) {
    assert(y >= 0 && y < height_);
    return pixels_.data() + step_ * y;
}
 
const Color* Image::GetLine(int y) const {return const_cast<Image*>(this)->GetLine(y);}
int Image::GetWidth() const {return width_;}
int Image::GetHeight() const {return height_;}
int Image::GetStep() const {return step_;}
 
}//end namespace img_lib
-----------------------------------------------------------------------
ppm_image.h
-----------------------------------------------------------------------
#pragma once
#include "img_lib.h"
 
#include <filesystem>
 
namespace img_lib {
    
using Path = std::filesystem::path;
 
bool SavePPM(const Path& file, const Image& image);
Image LoadPPM(const Path& file);
 
}//end namespace img_lib
-----------------------------------------------------------------------
ppm_image.cpp
-----------------------------------------------------------------------
#include "ppm_image.h"
 
#include <array>
#include <fstream>
#include <string_view>
 
using namespace std;
 
namespace img_lib {
 
static const string_view PPM_SIG = "P6"sv;
static const int PPM_MAX = 255;
 
bool SavePPM(const Path& file, const Image& image) {
    ofstream out(file, ios::binary);
 
    out << PPM_SIG << '\n' << image.GetWidth() << ' ' << image.GetHeight() << '\n' << PPM_MAX << '\n';
 
    const int w = image.GetWidth();
    const int h = image.GetHeight();
    std::vector<char> buff(w * 3);
 
    for (int y = 0; y < h; ++y) {
        
        const Color* line = image.GetLine(y);
        
        for (int x = 0; x < w; ++x) {
            buff[x * 3 + 0] = static_cast<char>(line[x].r);
            buff[x * 3 + 1] = static_cast<char>(line[x].g);
            buff[x * 3 + 2] = static_cast<char>(line[x].b);
        }
        
        out.write(buff.data(), w * 3);
    }
 
    return out.good();
}
 
Image LoadPPM(const Path& file) {
    ifstream ifs(file, ios::binary);
    std::string sign;
    int w, h, color_max;
 
    ifs >> sign >> w >> h >> color_max;
 
    if (sign != PPM_SIG || color_max != PPM_MAX) {return {};}
 
    const char next = ifs.get();
    if (next != '\n') {return {};}
 
    Image result(w, h, Color::Black());
    std::vector<char> buff(w * 3);
 
    for (int y = 0; y < h; ++y) {
        
        Color* line = result.GetLine(y);
        ifs.read(buff.data(), w * 3);
 
        for (int x = 0; x < w; ++x) {
            line[x].r = static_cast<byte>(buff[x * 3 + 0]);
            line[x].g = static_cast<byte>(buff[x * 3 + 1]);
            line[x].b = static_cast<byte>(buff[x * 3 + 2]);
        }
    }
 
    return result;
}
 
}//end namespace img_lib
