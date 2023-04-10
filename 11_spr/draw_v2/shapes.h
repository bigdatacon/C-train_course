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
        (void)texture_->PrintImage();
        //for (int y = position_.y; y < position_.y + size_.height; ++y) {
        for (int y = position_.y /*0*/; y < position_.y+ size_.height; ++y) {
            if (y > s.height) {
                std::string row(s.width, '.');
                std::cout << row << std::endl;
                break;
            }

            std::string& row = image[y];
            //for (int x = position_.x; x < position_.x + size_.width; ++x) {
            for (int x = position_.x-1 /*0*/; x < position_.x + size_.width; ++x) {
        
                if (! (x > s.width) ) {
                    char pixel = texture_->GetPixelColor({ x, y });
                    row[x] = pixel; 
                    image[y][x] = pixel;
                }
                else { 
                row[x] = '.';
                image[y][x] = '.';
                }     
            }
            std::cout << row << std::endl;
            
        }
    }

    void DrawEllipse(Image& image) const {
        Size s = texture_->GetSize();
        (void)texture_->PrintImage();
        //for (int y = position_.y; y < position_.y + size_.height; ++y) {
        for (int y= position_.y; y <= size_.height; ++y) {
            std::string& row = image[y];
            //for (int x = position_.x; x < position_.x + size_.width; ++x) {
            for (int x = position_.x; x <= size_.width; ++x) {

                if (IsPointInEllipse({ x, y }, s)) {

                    char pixel = texture_->GetPixelColor({ x, y });
                        row[x] = pixel;
                        image[y][x] = pixel;
                    }
                    else {
                        row[x] = '.';
                        image[y][x] = '.';
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
