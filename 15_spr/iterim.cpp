---------------------------------------------------------------------- -
CMakeLists.txt
---------------------------------------------------------------------- -
cmake_minimum_required(VERSION 3.11)

project(Negate CXX)
set(CMAKE_CXX_STANDARD 17)

add_executable("negate" "main.cpp" "img_lib.h" "img_lib.cpp" "ppm_image.h" "ppm_image.cpp")
---------------------------------------------------------------------- -
main.cpp
---------------------------------------------------------------------- -
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
---------------------------------------------------------------------- -
img_lib.h
---------------------------------------------------------------------- -
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
        static Color Black() { return { std::byte{0}, std::byte{0}, std::byte{0}, std::byte{255} }; }

        std::byte r, g, b, a;
    };

    class Image {
    public:

        Image() = default;

        Image(int w, int h, Color fill);

        Color GetPixel(int x, int y) const {
            return const_cast<Image*>(this)->GetPixel(x, y);
        }
        Color& GetPixel(int x, int y) {
            assert(x < GetWidth() && y < GetHeight() && x >= 0 && y >= 0);
            return GetLine(y)[x];
        }

        Color* GetLine(int y);
        const Color* GetLine(int y) const;

        int GetWidth() const;
        int GetHeight() const;

        int GetStep() const;

        explicit operator bool() const { return GetWidth() > 0 && GetHeight() > 0; }
        bool operator!() const { return !operator bool(); }

    private:
        int width_ = 0;
        int height_ = 0;
        int step_;

        std::vector<Color> pixels_;
    };

}//end namespace img_lib
---------------------------------------------------------------------- -
img_lib.cpp
---------------------------------------------------------------------- -
#include "img_lib.h"

namespace img_lib {

    Image::Image(int w, int h, Color fill) : width_(w)
        , height_(h)
        , step_(w)
        , pixels_(step_* height_, fill) {}

    Color* Image::GetLine(int y) {
        assert(y >= 0 && y < height_);
        return pixels_.data() + step_ * y;
    }

    const Color* Image::GetLine(int y) const { return const_cast<Image*>(this)->GetLine(y); }
    int Image::GetWidth() const { return width_; }
    int Image::GetHeight() const { return height_; }
    int Image::GetStep() const { return step_; }

}//end namespace img_lib
---------------------------------------------------------------------- -
ppm_image.h
---------------------------------------------------------------------- -
#pragma once
#include "img_lib.h"

#include <filesystem>

namespace img_lib {
    using Path = std::filesystem::path;

    bool SavePPM(const Path& file, const Image& image);
    Image LoadPPM(const Path& file);

}//end namespace img_lib
---------------------------------------------------------------------- -
ppm_image.cpp
---------------------------------------------------------------------- -
#include "ppm_image.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib {

    static const string_view PPM_SIG = "P6"sv;
    static const int PPM_MAX = 255;

    bool SavePPM(const Path& file, const Image& image) {
        ofstream ofs(file, ios::out | ios::binary);
        int w{ image.GetWidth() };
        int h{ image.GetHeight() };
        int color_max{ PPM_MAX };

        ofs << PPM_SIG << '\n' << w << ' ' << h << '\n' << color_max << '\n';

        Image result{ image };
        std::vector<char> buff(w * 3);

        for (int y = 0; y < h; ++y) {

            Color* line = result.GetLine(y);
            for (int x = 0; x < w; ++x) {
                buff[x * 3 + 0] = static_cast<char>(line[x].r);
                buff[x * 3 + 1] = static_cast<char>(line[x].g);
                buff[x * 3 + 2] = static_cast<char>(line[x].b);
            }

            ofs.write(buff.data(), w * 3);

            if (ofs.fail()) {
                return false;
            }
        }

        return true;
    }

    Image LoadPPM(const Path& file) {
        ifstream ifs(file, ios::binary);
        std::string sign;
        int w, h, color_max;

        ifs >> sign >> w >> h >> color_max;

        if (sign != PPM_SIG || color_max != PPM_MAX) { return {}; }

        const char next = ifs.get();
        if (next != '\n') { return {}; }

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
