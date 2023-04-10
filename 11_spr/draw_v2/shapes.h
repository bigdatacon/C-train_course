#pragma once
#include "texture.h"

#include <memory>
#include <iostream>
#include <cmath>

// Поддерживаемые виды фигур: прямоугольник и эллипс
enum class ShapeType { RECTANGLE, ELLIPSE };


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
        if (type_ == ShapeType::RECTANGLE) {
            DrawRectangle(image);
        }
        else if (type_ == ShapeType::ELLIPSE) {
            DrawEllipse(image);
        }
    }

    void DrawRectangle(Image& image) const {
        Size s = texture_->GetSize();
        for (int y = position_.y; y < position_.y + size_.height; ++y) {
            std::string& row = image[y];
            for (int x = position_.x; x < position_.x + size_.width; ++x) {
        
                if (! (x > s.width || y > s.height) ) {
                    char pixel = texture_->GetPixelColor({ x, y });
                    row[x] = pixel; 
                }
                else { 
                row[x] = '.';
                }     
            }
            std::cout << row << std::endl;
            
        }
    }

    void DrawEllipse(Image& image) const {
        Size s = texture_->GetSize();
        for (int y = position_.y; y < position_.y + size_.height; ++y) {
            std::string& row = image[y];
            for (int x = position_.x; x < position_.x + size_.width; ++x) {

                if (IsPointInEllipse({ x, y }, s)) {

                    char pixel = texture_->GetPixelColor({ x, y });
                        row[x] = pixel;
                    }
                    else {
                        row[x] = '.';
                    }
                }
            
            std::cout << row << std::endl;
        }
    }


private:
    ShapeType type_;
    Point position_;
    Size size_;
    std::shared_ptr<Texture> texture_;

};