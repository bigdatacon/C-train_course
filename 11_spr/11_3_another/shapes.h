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
        Size image_size = GetImageSize(image);
        // Начало рисования по оси Х: точка pos_.x
        int start_draw_to_x = position_.x;
        // Начало рисования по оси Y: точка pos_.y
        int start_draw_to_y = position_.y;
        // Конец рисования по оси Х: ограничена размером фигуры (size_.width), если фигура выходит за пределы холста (image_size.width), то конец - размер холста.
        int end_draw_to_x = size_.width > image_size.width ? image_size.width - 1 : size_.width + position_.x - 1;
        // Конец рисования по оси Y: ограничена размером фигуры (size_.height), если фигура выходит за пределы холста (image_size.height), то конец - размер холста.
        int end_draw_to_y = size_.height > image_size.height ? image_size.height - 1 : size_.height + position_.y;

        // Вводим новые переменные t_x и t_y
        // t_x: отвечает за взятие координаты текстуры по горизонтали (ширина строки)
        // t_y: отвечает за взятие координаты текстуры по вертикали (индекс строки)
        Point t;
        t.x = 0; // t_y не может быть больше texture_.GetSize().width
        t.y = 0; // t_y не может быть больше texture_.GetSize().height

        // Если текстура не задана, рисуем точки. Если размер текстуры меньше размера фигуры, при выходе за границы рисуем точки
        if (type_ == ShapeType::RECTANGLE) {
            // Если текстуры нет
            if (!texture_) {
                // Во внутреннем цикле перебираем Х от start до end
                // Во внешнем цикле перебираем Y от start до end
                for (int i = start_draw_to_y; i < end_draw_to_y; i++) {
                    for (int j = start_draw_to_x; j <= end_draw_to_x; j++) {
                        // Првоеряем, входит ли указанная координата в размер фигуры
                        //if (IsPointInRectangle(t, size_)) {
                            // Если точка входит в фигуру, рисуем символ '.'
                            image[i][j] = '.';
                        //}
                        // После того, как нарисовали или проигнорировали пиксель, увеличиваем t.x
                        t.x++;
                    }
                    // Когда внутренний цикл завершен, t.x сбрасывается
                    // А t.y увеличивается на 1
                    t.x = 0;
                    t.y++;
                }
                // Когда все циклы завершены, сбрасываем точку
                t.x = 0;
                t.y = 0;
            }
            else {
                Size texture_size = texture_->GetSize();
                // Во внутреннем цикле перебираем Х от start до end
                // Во внешнем цикле перебираем Y от start до end
                for (int i = start_draw_to_y; i < end_draw_to_y; i++) {
                    for (int j = start_draw_to_x; j <= end_draw_to_x; j++) {
                        // Если ширина текстуры < ширины фигуры ИЛИ высота текстуры < высоты фигуры
                        // Т.е. если при рисовании фигуры выходим за пределы текстуры, рисуем '.'
                        // При этом учитываем вхождение в заданный прямоугольник
                        if (t.y >= texture_size.height || t.x >= texture_size.width) {
                            //if (IsPointInRectangle(t, size_)) {
                                image[i][j] = '.';
                            //}
                        }
                        else {
                            // Если за пределы текстуры НЕ выходим, то спрашиваем у текстуры цвет пикселя
                            // Предварительно проверив, входят ли указанные координаты в фигуру
                            //if (IsPointInRectangle(t, size_)) {
                            image[i][j] = texture_->GetPixelColor(t);
                            //}
                        }
                        // После того, как нарисовали или проигнорировали пиксель, увеличиваем t.x
                        t.x++;
                    }
                    // Когда внутренний цикл завершен, t.x сбрасывается
                    // А t.y увеличивается на 1
                    t.x = 0;
                    t.y++;
                }
                // Когда все циклы завершены, сбрасываем точку
                t.x = 0;
                t.y = 0;
            }
        }
        // Такая же херня, как и с прямоугольником, только учитываем, что это ЭЛЛИПС
        // Если текстура не задана, рисуем точки. Если размер текстуры меньше размера фигуры, при выходе за границы рисуем точки
        if (type_ == ShapeType::ELLIPSE) {
            // Если текстуры нет
            if (!texture_) {
                // Во внутреннем цикле перебираем Х от start до end
                // Во внешнем цикле перебираем Y от start до end
                for (int i = start_draw_to_y; i < end_draw_to_y; i++) {
                    for (int j = start_draw_to_x; j <= end_draw_to_x; j++) {
                        // Если точка t входит в фигуру, рисуем '.'
                        if (IsPointInEllipse(t, size_)) {
                            image[i][j] = '.';
                        }
                        // После того, как нарисовали или проигнорировали пиксель, увеличиваем t.x
                        t.x++;
                    }
                    // Когда внутренний цикл завершен, t.x сбрасывается
                    // А t.y увеличивается на 1
                    t.x = 0;
                    t.y++;
                }
                // Когда все циклы завершены, сбрасываем точку
                t.x = 0;
                t.y = 0;
            }
            else {
                Size texture_size = texture_->GetSize();
                // Если текстура есть
                // Во внутреннем цикле перебираем Х от start до end
                // Во внешнем цикле перебираем Y от start до end
                for (int i = start_draw_to_y; i < end_draw_to_y; i++) {
                    for (int j = start_draw_to_x; j <= end_draw_to_x; j++) {
                        // Если ширина текстуры < ширины фигуры ИЛИ высота текстуры < высоты фигуры
                        // Т.е. если при рисовании фигуры выходим за пределы текстуры, рисуем '.'
                        // При этом учитываем вхождение в эллипс
                        if (t.y >= texture_size.height || t.x >= texture_size.width) {
                            if (IsPointInEllipse(t, size_)) {
                                image[i][j] = '.';
                            }
                        }
                        else {
                            // Если за пределы текстуры НЕ выходим, то спрашиваем у текстуры цвет пикселя
                            // Предварительно проверив, входят ли указанные координаты в фигуру
                            if (IsPointInEllipse(t, size_)) {
                                image[i][j] = texture_->GetPixelColor(t);
                            }
                        }
                        // После того, как нарисовали или проигнорировали пиксель, увеличиваем t.x
                        t.x++;
                    }
                    // Когда внутренний цикл завершен, t.x сбрасывается
                    // А t.y увеличивается на 1
                    t.x = 0;
                    t.y++;
                }
                // Когда все циклы завершены, сбрасываем точку
                t.x = 0;
                t.y = 0;
            }
        }
    }


private:
    ShapeType type_;
    Point position_;
    Size size_;
    std::shared_ptr<Texture> texture_;

};