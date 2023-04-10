#pragma once

#include <string>
#include <vector>

struct Size {
    int width = 0;
    int height = 0;
};

struct Point {
    int x = 0;
    int y = 0;
};

// �����������. ������� ��� �������.
// ������ ������ (�� std::vector) - ������ �����������, ���������� y
// ������ ������ (�� std::string) - ������� �����������, ���������� x
// ��������������, ��� ����� ���� ����� ���������
using Image = std::vector<std::string>;

inline Size GetImageSize(const Image& image) {
    const int width = image.empty() ? 0 : static_cast<int>(image[0].size());
    const int height = static_cast<int>(image.size());
    return { width, height };
}

// ���������, ���������� �� �������� ����� � ������� ��������� �������
// ���������, ��� ������ ������ � ������������� � ��������� � ������ (0, 0) �
// (size.width, size.height)
inline bool IsPointInEllipse(Point p, Size size) {
    // ��������� ���������� ����� � �������� (-1, 1)
    // ����� �� 0.5 �����, ����� ������� ���������� �� ������ �������, ���
    // ���������� ����� �������� �����
    double x = (p.x + 0.5) / (size.width / 2.0) - 1;
    double y = (p.y + 0.5) / (size.height / 2.0) - 1;
    // ���������, ����� �� ����� � ��������� �����
    return x * x + y * y <= 1;
}