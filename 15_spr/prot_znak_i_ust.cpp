 
*** PACKAGE ImgConverter ***
 
-----------------------------------------------------------------------
CMakeLists.txt
-----------------------------------------------------------------------
cmake_minimum_required(VERSION 3.11)
 
project(Negate CXX)
set(CMAKE_CXX_STANDARD 17)
 
add_subdirectory(../ImgLib ImgLibBuildDir)
 
if (CMAKE_SYSTEM_NAME MATCHES "^MINGW")
    set(SYSTEM_LIBS -lstdc++)
else()
    set(SYSTEM_LIBS)
endif()
 
add_executable(imgconv main.cpp)
 
target_include_directories(imgconv PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../ImgLib")
target_link_libraries(imgconv ImgLib ${SYSTEM_LIBS})
-----------------------------------------------------------------------
main.cpp
-----------------------------------------------------------------------
#include <bmp_image.h>
#include <jpeg_image.h>
#include <ppm_image.h>
 
#include <filesystem>
#include <iostream>
#include <string_view>
 
using namespace std;
 
enum class Format { JPEG, PPM, BMP, UNKNOWN };
 
Format GetFormatByExtension(const img_lib::Path& input_file) {
    const string ext = input_file.extension().string();
    
    if (ext == ".jpg"sv || ext == ".jpeg"sv) {
        return Format::JPEG;
    }
    if (ext == ".ppm"sv) {
        return Format::PPM;
    }
    if (ext == ".bmp"sv) {
        return Format::BMP;
    }
 
    return Format::UNKNOWN;
}
 
class ImageFormatInterface {
public:
    virtual bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const = 0;
    virtual img_lib::Image LoadImage(const img_lib::Path& file) const = 0;
};
 
namespace format_interfaces {
 
class PPM : public ImageFormatInterface {
public:
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
        return img_lib::SavePPM(file, image);
    }
 
    img_lib::Image LoadImage(const img_lib::Path& file) const override {
        return img_lib::LoadPPM(file);
    }
};
 
class JPEG : public ImageFormatInterface {
public:
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
        return img_lib::SaveJPEG(file, image);
    }
 
    img_lib::Image LoadImage(const img_lib::Path& file) const override {
        return img_lib::LoadJPEG(file);
    }
};
 
class BMP : public ImageFormatInterface {
public:
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
        return img_lib::SaveBMP(file, image);
    }
 
    img_lib::Image LoadImage(const img_lib::Path& file) const override {
        return img_lib::LoadBMP(file);
    }
};
 
}//end namespace format_interfaces
 
const ImageFormatInterface* GetFormatInterface(const img_lib::Path& path) {
    
    Format format = GetFormatByExtension(path);
    
    if (format == Format::UNKNOWN) {
        return nullptr;
    }
 
    static const format_interfaces::PPM ppmInterface;
    static const format_interfaces::JPEG jpegInterface;
    static const format_interfaces::BMP bmpInterface;
 
    switch (format) {
        case Format::UNKNOWN:
            return nullptr;
 
        case Format::PPM:
            return &ppmInterface;
 
        case Format::JPEG:
            return &jpegInterface;
 
        case Format::BMP:
            return &bmpInterface;
    }
 
    return nullptr;
}
 
int main(int argc, const char** argv) {
    
    if (argc != 3) {
        cerr << "Usage: "sv << argv[0] << " <in_file> <out_file>"sv << endl;
        return 1;
    }
 
    img_lib::Path in_path = argv[1];
    img_lib::Path out_path = argv[2];
 
    const ImageFormatInterface* in_format_interface = GetFormatInterface(in_path);
    if (!in_format_interface) {
        cerr << "Unknown format of the input file"sv << endl;
        return 2;
    }
 
    const ImageFormatInterface* out_format_interface = GetFormatInterface(out_path);
    if (!out_format_interface) {
        cerr << "Unknown format of the output file"sv << endl;
        return 3;
    }
 
    img_lib::Image image = in_format_interface->LoadImage(in_path);
    if (!image) {
        cerr << "Loading failed"sv << endl;
        return 4;
    }
 
    if (!out_format_interface->SaveImage(out_path, image)) {
        cerr << "Saving failed"sv << endl;
        return 5;
    }
 
    cout << "Successfully converted"sv << endl;
}
 
*** PACKAGE ImgConverter END ***
 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 
*** PACKAGE ImgLib ***
 
-----------------------------------------------------------------------
CMakeLists.txt
-----------------------------------------------------------------------
cmake_minimum_required(VERSION 3.11)
 
project(ImgLib CXX)
set(CMAKE_CXX_STANDARD 17)
 
set(LIBJPEG_DIR CACHE STRING "LibJPEG static library directory")
 
if(NOT LIBJPEG_DIR)
    message(FATAL_ERROR "Please, specify LibJPEG directory via -DLIBJPEG_DIR=<dir>")
endif()
    message(STATUS "LibJPEG dir is ${LIBJPEG_DIR}, change via -DLIBJPEG_DIR=<dir>")
 
set(IMGLIB_MAIN_FILES img_lib.h img_lib.cpp)
 
set(IMGLIB_FORMAT_FILES ppm_image.h ppm_image.cpp 
                        jpeg_image.h jpeg_image.cpp
                        bmp_image.h bmp_image.cpp)
 
add_library(ImgLib STATIC ${IMGLIB_MAIN_FILES} 
                          ${IMGLIB_FORMAT_FILES})
 
target_include_directories(ImgLib PUBLIC "${LIBJPEG_DIR}/include")
target_link_directories(ImgLib INTERFACE "${LIBJPEG_DIR}/lib/$<IF:$<CONFIG:Debug>,Debug,Release>")
target_link_libraries(ImgLib INTERFACE jpeg)
-----------------------------------------------------------------------
jpeg_image.h
-----------------------------------------------------------------------
#pragma once
#include "img_lib.h"
 
#include <filesystem>
 
namespace img_lib {
    
using Path = std::filesystem::path;
 
bool SaveJPEG(const Path& file, const Image& image);
Image LoadJPEG(const Path& file);
 
}//end namespace img_lib
-----------------------------------------------------------------------
jpeg_image.cpp
-----------------------------------------------------------------------
#include "ppm_image.h"
 
#include <array>
#include <fstream>
#include <stdio.h>
#include <setjmp.h>
#include <vector>
 
#include <jpeglib.h>
 
using namespace std;
 
namespace img_lib {
 
struct my_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};
 
typedef struct my_error_mgr* my_error_ptr;
 
METHODDEF(void)
my_error_exit (j_common_ptr cinfo) {
    my_error_ptr myerr = (my_error_ptr) cinfo->err;
    (*cinfo->err->output_message) (cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}
 
bool SaveJPEG(const Path& file, const Image& image) {
 
    jpeg_compress_struct cinfo;
    jpeg_error_mgr jerr;
 
    FILE* outfile;              
    JSAMPROW row_pointer[1];     
    int row_stride;                
 
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
 
#ifdef _MSC_VER
    if ((outfile = _wfopen(file.wstring().c_str(), "wb")) == NULL) {
#else
    if ((outfile = fopen(file.string().c_str(), "wb")) == NULL) {
#endif
    return false;}
    
    jpeg_stdio_dest(&cinfo, outfile);
 
    cinfo.image_width = image.GetWidth(); 
    cinfo.image_height = image.GetHeight();
    cinfo.input_components = 3;       
    cinfo.in_color_space = JCS_RGB;   
 
    jpeg_set_defaults(&cinfo);
    jpeg_start_compress(&cinfo, TRUE);
    row_stride = image.GetWidth() * 3;
 
    while (cinfo.next_scanline < cinfo.image_height) {       
        std::vector<JSAMPLE> jsample(row_stride);
        
        for (int i = 0; i < image.GetWidth(); ++i) {
            
            jsample[3 * i] = JSAMPLE(image.GetLine(cinfo.next_scanline)[i].r);
            jsample[1 + 3 * i] = JSAMPLE(image.GetLine(cinfo.next_scanline)[i].g);
            jsample[2 + 3 * i] = JSAMPLE(image.GetLine(cinfo.next_scanline)[i].b);
        }
        
        row_pointer[0] = &jsample[0];
        (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
 
    jpeg_finish_compress(&cinfo);
    fclose(outfile);
 
    jpeg_destroy_compress(&cinfo);
    
    return true;
}
 
void SaveScanlineToImage(const JSAMPLE* row, int y, Image& out_image) {
    
    Color* line = out_image.GetLine(y);
    
    for (int x = 0; x < out_image.GetWidth(); ++x) {
        
        const JSAMPLE* pixel = row + x * 3;
        line[x] = Color{byte{pixel[0]}, byte{pixel[1]}, byte{pixel[2]}, byte{255}};
    }
}
 
Image LoadJPEG(const Path& file) {
    jpeg_decompress_struct cinfo;
    my_error_mgr jerr;
    
    FILE* infile;
    JSAMPARRAY buffer;
    int row_stride;
 
#ifdef _MSC_VER
    if ((infile = _wfopen(file.wstring().c_str(), "rb")) == NULL) {
#else
    if ((infile = fopen(file.string().c_str(), "rb")) == NULL) {
#endif
        return {};
    }
 
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
 
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return {};
    }
 
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    (void) jpeg_read_header(&cinfo, TRUE);
    cinfo.out_color_space = JCS_RGB;
    cinfo.output_components = 3;
    (void) jpeg_start_decompress(&cinfo);
    
    row_stride = cinfo.output_width * cinfo.output_components;    
    buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
 
    Image result(cinfo.output_width, cinfo.output_height, Color::Black());
 
    while (cinfo.output_scanline < cinfo.output_height) {
        int y = cinfo.output_scanline;
        (void) jpeg_read_scanlines(&cinfo, buffer, 1);
 
        SaveScanlineToImage(buffer[0], y, result);
    }
 
    (void) jpeg_finish_decompress(&cinfo);
 
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
 
    return result;
}
 
}//end namespace img_lib
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
 
    const int width = image.GetWidth();
    const int height = image.GetHeight();
    
    std::vector<char> buff(width * 3);
 
    for (int i = 0; i < height; ++i) {
        
        const Color* line = image.GetLine(i);
        
        for (int j = 0; j < width; ++j) {
            buff[3 * j] = static_cast<char>(line[j].r);
            buff[1 + 3 * j] = static_cast<char>(line[j].g);
            buff[2 + 3 * j] = static_cast<char>(line[j].b);
        }
        
        out.write(buff.data(), width * 3);
    }
 
    return out.good();
}
 
Image LoadPPM(const Path& file) {
    
    ifstream ifs(file, ios::binary);
    
    std::string sign;
    
    int width;
    int height; 
    int color_max;
    auto color = Color::Black();
    
    ifs >> sign >> width >> height >> color_max;
 
    if (sign != PPM_SIG || color_max != PPM_MAX) {return {};}
 
    const char next = ifs.get();
    if (next != '\n') {return {};}
 
    Image result(width, height, color);
    std::vector<char> buff(width * 3);
 
    for (int i = 0; i < height; ++i) {
        
        Color* line = result.GetLine(i);
        ifs.read(buff.data(), width * 3);
 
        for (int j = 0; j < width; ++j) {
            line[j].r = static_cast<byte>(buff[3 * j]);
            line[j].g = static_cast<byte>(buff[1 + 3 * j]);
            line[j].b = static_cast<byte>(buff[2 + 3 * j]);
        }
    }
 
    return result;
}
 
}//end namespace img_lib
-----------------------------------------------------------------------
bmp_image.h
-----------------------------------------------------------------------
#pragma once
#include "img_lib.h"
 
#include <filesystem>
 
namespace img_lib {
    
using Path = std::filesystem::path;
 
bool SaveBMP(const Path& file, const Image& image);
Image LoadBMP(const Path& file);
 
}//end namespace img_lib
-----------------------------------------------------------------------
bmp_image.cpp
-----------------------------------------------------------------------
#include "bmp_image.h"
#include "pack_defines.h"
 
#include <array>
#include <fstream>
#include <string_view>
 
using namespace std;
 
namespace img_lib {
 
PACKED_STRUCT_BEGIN BitmapFileHeader {
    char sign1 = 'B'; 
    char sign2 = 'M'; 
    
    uint32_t header_size = 0;
    
    uint32_t reserved_space = 0;
    uint32_t indention = 54;
    
} PACKED_STRUCT_END
 
PACKED_STRUCT_BEGIN BitmapInfoHeader {
    uint32_t header_size = 40;
    
    uint32_t width = 0; 
    uint32_t height = 0;
    
    uint16_t layers = 1;
    uint16_t bit_per_pixel = 24;
    
    uint32_t compression_type = 0;
    uint32_t bytes_at_data = 0;
    
    int32_t vertical_pixel_per_meter = 11811;
    int32_t horizontal_pixel_per_meter = 11811;
    
    int32_t colors_in_use = 0;
    int32_t colors = 0x1000000;
    
} PACKED_STRUCT_END
 
static int GetBMPStride(int w) {return 4 * ((w * 3 + 3) / 4);}
 
bool SaveBMP(const Path& file, const Image& image) {
    
    ofstream out(file, ios::binary);
    
    int width = image.GetWidth();
    int height = image.GetHeight();
 
    const int bmp_stride = GetBMPStride(width);
    
    BitmapFileHeader file_header;
    BitmapInfoHeader info_header;
    
    file_header.header_size = bmp_stride * height + file_header.indention;
    
    info_header.width = width;
    info_header.height = height;
    info_header.bytes_at_data = height * bmp_stride;
 
    out.write(reinterpret_cast<const char*>(&file_header), 14);
    out.write(reinterpret_cast<const char*>(&file_header), 40);
 
    std::vector<char> buff(bmp_stride);
    
    for (int i = height - 1; i >= 0; --i) {
        const Color* line = image.GetLine(i);
        
        for (int j = 0; j != width; ++j) {
            
            buff[3 * j] = static_cast<char>(line[j].b);
            buff[1 + 3 * j] = static_cast<char>(line[j].g);
            buff[2 + 3 * j] = static_cast<char>(line[j].r);
        }
        
        out.write(buff.data(), bmp_stride);
    }
 
    return out.good();
}
 
Image LoadBMP(const Path& file) {
    
    ifstream input(file, ios::binary);
 
    BitmapFileHeader file_header;
    BitmapInfoHeader info_header;
 
    input.read(reinterpret_cast<char*>(&file_header), sizeof(BitmapFileHeader));
    input.read(reinterpret_cast<char*>(&info_header), sizeof(BitmapInfoHeader));
 
    int width = info_header.width;
    int height = info_header.height;
    auto color = Color::Black();
 
    const int bmp_stride = GetBMPStride(width);
    
    std::vector<char> buff(bmp_stride);
    Image result(width, width, color);
 
    for (int i = height - 1; i >= 0; --i) {
        
        input.read(buff.data(), bmp_stride);
        Color* line = result.GetLine(i);
        
        for (int j = 0; j != width; ++j) {
            line[j].b = static_cast<byte>(buff[j * 3]);
            line[j].g = static_cast<byte>(buff[1 + j * 3]);
            line[j].r = static_cast<byte>(buff[2 + j * 3]);
        }
    }
 
    return result;
}
 
}//end namespace img_lib
-----------------------------------------------------------------------
pack_defines.h
-----------------------------------------------------------------------
#pragma once
 
#if !defined(ALIGNED_TYPE_BEGIN)
    #if defined(_MSC_VER) || defined(__INTEL_COMPILER_BUILD_DATE)
        #define PACKED_STRUCT_BEGIN \
            __pragma(pack(push,1)) \
            struct __declspec(align(1))
        #define PACKED_STRUCT_END \
            ;__pragma(pack(pop))
    #else
        #define PACKED_STRUCT_BEGIN \
            struct
        #define PACKED_STRUCT_END  __attribute__((packed));
    #endif
#endif
 
*** PACKAGE ImgLib END ***
