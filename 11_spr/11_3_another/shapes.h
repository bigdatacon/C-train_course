#pragma once
#include "texture.h"

#include <memory>
#include <iostream>
#include <cmath>

// �������������� ���� �����: ������������� � ������
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
        // ������ ��������� �� ��� �: ����� pos_.x
        int start_draw_to_x = position_.x;
        // ������ ��������� �� ��� Y: ����� pos_.y
        int start_draw_to_y = position_.y;
        // ����� ��������� �� ��� �: ���������� �������� ������ (size_.width), ���� ������ ������� �� ������� ������ (image_size.width), �� ����� - ������ ������.
        int end_draw_to_x = size_.width > image_size.width ? image_size.width - 1 : size_.width + position_.x - 1;
        // ����� ��������� �� ��� Y: ���������� �������� ������ (size_.height), ���� ������ ������� �� ������� ������ (image_size.height), �� ����� - ������ ������.
        int end_draw_to_y = size_.height > image_size.height ? image_size.height - 1 : size_.height + position_.y;

        // ������ ����� ���������� t_x � t_y
        // t_x: �������� �� ������ ���������� �������� �� ����������� (������ ������)
        // t_y: �������� �� ������ ���������� �������� �� ��������� (������ ������)
        Point t;
        t.x = 0; // t_y �� ����� ���� ������ texture_.GetSize().width
        t.y = 0; // t_y �� ����� ���� ������ texture_.GetSize().height

        // ���� �������� �� ������, ������ �����. ���� ������ �������� ������ ������� ������, ��� ������ �� ������� ������ �����
        if (type_ == ShapeType::RECTANGLE) {
            // ���� �������� ���
            if (!texture_) {
                // �� ���������� ����� ���������� � �� start �� end
                // �� ������� ����� ���������� Y �� start �� end
                for (int i = start_draw_to_y; i < end_draw_to_y; i++) {
                    for (int j = start_draw_to_x; j <= end_draw_to_x; j++) {
                        // ���������, ������ �� ��������� ���������� � ������ ������
                        //if (IsPointInRectangle(t, size_)) {
                            // ���� ����� ������ � ������, ������ ������ '.'
                            image[i][j] = '.';
                        //}
                        // ����� ����, ��� ���������� ��� ��������������� �������, ����������� t.x
                        t.x++;
                    }
                    // ����� ���������� ���� ��������, t.x ������������
                    // � t.y ������������� �� 1
                    t.x = 0;
                    t.y++;
                }
                // ����� ��� ����� ���������, ���������� �����
                t.x = 0;
                t.y = 0;
            }
            else {
                Size texture_size = texture_->GetSize();
                // �� ���������� ����� ���������� � �� start �� end
                // �� ������� ����� ���������� Y �� start �� end
                for (int i = start_draw_to_y; i < end_draw_to_y; i++) {
                    for (int j = start_draw_to_x; j <= end_draw_to_x; j++) {
                        // ���� ������ �������� < ������ ������ ��� ������ �������� < ������ ������
                        // �.�. ���� ��� ��������� ������ ������� �� ������� ��������, ������ '.'
                        // ��� ���� ��������� ��������� � �������� �������������
                        if (t.y >= texture_size.height || t.x >= texture_size.width) {
                            //if (IsPointInRectangle(t, size_)) {
                                image[i][j] = '.';
                            //}
                        }
                        else {
                            // ���� �� ������� �������� �� �������, �� ���������� � �������� ���� �������
                            // �������������� ��������, ������ �� ��������� ���������� � ������
                            //if (IsPointInRectangle(t, size_)) {
                            image[i][j] = texture_->GetPixelColor(t);
                            //}
                        }
                        // ����� ����, ��� ���������� ��� ��������������� �������, ����������� t.x
                        t.x++;
                    }
                    // ����� ���������� ���� ��������, t.x ������������
                    // � t.y ������������� �� 1
                    t.x = 0;
                    t.y++;
                }
                // ����� ��� ����� ���������, ���������� �����
                t.x = 0;
                t.y = 0;
            }
        }
        // ����� �� �����, ��� � � ���������������, ������ ���������, ��� ��� ������
        // ���� �������� �� ������, ������ �����. ���� ������ �������� ������ ������� ������, ��� ������ �� ������� ������ �����
        if (type_ == ShapeType::ELLIPSE) {
            // ���� �������� ���
            if (!texture_) {
                // �� ���������� ����� ���������� � �� start �� end
                // �� ������� ����� ���������� Y �� start �� end
                for (int i = start_draw_to_y; i < end_draw_to_y; i++) {
                    for (int j = start_draw_to_x; j <= end_draw_to_x; j++) {
                        // ���� ����� t ������ � ������, ������ '.'
                        if (IsPointInEllipse(t, size_)) {
                            image[i][j] = '.';
                        }
                        // ����� ����, ��� ���������� ��� ��������������� �������, ����������� t.x
                        t.x++;
                    }
                    // ����� ���������� ���� ��������, t.x ������������
                    // � t.y ������������� �� 1
                    t.x = 0;
                    t.y++;
                }
                // ����� ��� ����� ���������, ���������� �����
                t.x = 0;
                t.y = 0;
            }
            else {
                Size texture_size = texture_->GetSize();
                // ���� �������� ����
                // �� ���������� ����� ���������� � �� start �� end
                // �� ������� ����� ���������� Y �� start �� end
                for (int i = start_draw_to_y; i < end_draw_to_y; i++) {
                    for (int j = start_draw_to_x; j <= end_draw_to_x; j++) {
                        // ���� ������ �������� < ������ ������ ��� ������ �������� < ������ ������
                        // �.�. ���� ��� ��������� ������ ������� �� ������� ��������, ������ '.'
                        // ��� ���� ��������� ��������� � ������
                        if (t.y >= texture_size.height || t.x >= texture_size.width) {
                            if (IsPointInEllipse(t, size_)) {
                                image[i][j] = '.';
                            }
                        }
                        else {
                            // ���� �� ������� �������� �� �������, �� ���������� � �������� ���� �������
                            // �������������� ��������, ������ �� ��������� ���������� � ������
                            if (IsPointInEllipse(t, size_)) {
                                image[i][j] = texture_->GetPixelColor(t);
                            }
                        }
                        // ����� ����, ��� ���������� ��� ��������������� �������, ����������� t.x
                        t.x++;
                    }
                    // ����� ���������� ���� ��������, t.x ������������
                    // � t.y ������������� �� 1
                    t.x = 0;
                    t.y++;
                }
                // ����� ��� ����� ���������, ���������� �����
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