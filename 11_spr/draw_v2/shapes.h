#pragma once
#include "texture.h"

#include <memory>
#include <cmath>

// Поддерживаемые виды фигур: прямоугольник и эллипс
enum class ShapeType { RECTANGLE, ELLIPSE};


class Shape {
public:
    explicit Shape(ShapeType type) : type_(type) {}

    void SetPosition(Point pos) {
        position_ = pos;
    }

    void SetSize(Size size) {
        size_ = size;
    }

    void SetTexture(std::shared_ptr<Texture> texture) {
        texture_ = texture;
    }

    void Draw(Image& image) const {
        for (int y = position_.y; y < position_.y + size_.height; ++y) {
             if (y >= image.size()) {
                break;
            } 
            std::string& row = image[y];
            for (int x = position_.x; x < position_.x + size_.width; ++x) {
                if (x >= row.size()) {
                    break;
                }
                char pixel =  texture_->GetPixelColor({x, y});
                row[x] = pixel;
            }
         cout << row << endl;   
        }
    }

private:
    ShapeType type_;
    Point position_;
    Size size_;
    std::shared_ptr<Texture> texture_;          
