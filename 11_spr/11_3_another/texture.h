#pragma once
#include "common.h"
#include <iostream>

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
        return image_[p.y][p.x]; 
    }


    void PrintImage() {
        for (auto el : image_)
        {
            std::cout << "THIS STTRING :" << el << std::endl;
        }
    }


private:
    Image image_;
};