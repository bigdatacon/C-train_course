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
        Size s = /*texture_->GetSize();*/ GetImageSize(image);
        (void)texture_->PrintImage();
        int start_draw_to_x = position_.x;
        // Начало рисования по оси Y: точка pos_.y
        int start_draw_to_y = position_.y;
        // Конец рисования по оси Х: ограничена размером фигуры (size_.width), если фигура выходит за пределы холста (image_size.width), то конец - размер холста.
        int end_draw_to_x = size_.width > s.width ? s.width - 1 : size_.width + position_.x - 1;
        // Конец рисования по оси Y: ограничена размером фигуры (size_.height), если фигура выходит за пределы холста (image_size.height), то конец - размер холста.
        int end_draw_to_y = size_.height > s.height ? s.height - 1 : size_.height + position_.y;
 
        for (int y = position_.y; y < end_draw_to_y + size_.height; ++y) {
            if (y > s.height) {
                std::string row(s.width, '.');
                std::cout << row << std::endl;
                break;
            }

            std::string& row = image[y];
            //for (int x = position_.x; x < position_.x + size_.width; ++x) {
            for (int x = position_.x ; x < end_draw_to_x; ++x) {
        
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
        int start_draw_to_x = position_.x;
        // Начало рисования по оси Y: точка pos_.y
        int start_draw_to_y = position_.y;
        // Конец рисования по оси Х: ограничена размером фигуры (size_.width), если фигура выходит за пределы холста (image_size.width), то конец - размер холста.
        int end_draw_to_x = size_.width > s.width ? s.width - 1 : size_.width + position_.x - 1;
        // Конец рисования по оси Y: ограничена размером фигуры (size_.height), если фигура выходит за пределы холста (image_size.height), то конец - размер холста.
        int end_draw_to_y = size_.height > s.height ? s.height - 1 : size_.height + position_.y;


        //for (int y = position_.y; y < position_.y + size_.height; ++y) {
        for (int y= position_.y; y <= end_draw_to_y; ++y) {
            std::string& row = image[y];
            //for (int x = position_.x; x < position_.x + size_.width; ++x) {
            for (int x = position_.x; x <= end_draw_to_x; ++x) {

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
