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


    /*char GetPixelColor(Point p) const {
        Size s = GetSize();
        if (!IsPointInEllipse(p, s)) {
            return '.';
        }
        else { return image_[p.y][p.x]; }
    }*/

    char GetPixelColor(Point p) const {
        return image_[p.y][p.x]; 
    }



private:
    Image image_;
};
