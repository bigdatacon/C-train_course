#pragma once
#include "common.h"

class Texture {
public:
    explicit Texture(Image image)
        : image_(std::move(image)) {
    }

    Size GetSize() const {
        // Заглушка. Реализуйте метод самостоятельно
        return GetImageSize(image_);
    }


    char GetPixelColor(Point p) const {
        Size s = GetSize();
        if (!IsPointInEllipse(p, s)) {
            return '.';
        }
        else { return image_[p.y][p.x]; }
    }

    /*
     char GetPixelColor(Point p) const {
     const auto [x, y] = p;
     const auto [width, height] = GetSize();
     if (x < 0 || y < 0 || x >= width || y >= height) {
         return '.';
     }
     return image_[y][x];
 }*/



private:
    Image image_;
