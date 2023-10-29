

#include "helper.h"
#include <iostream>
#include <cmath>
#include <limits>

Point::Point() : x(0), y(0) {}
Point::Point(double x, double y) : x(x), y(y) {}

bool operator==(const Point& lhs, const Point& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator!=(const Point& lhs, const Point& rhs) {
    return !(lhs == rhs);
}

LineSegment::LineSegment(const Point& start, const Point& end) : start(start), end(end) {}

bool operator<(const LineSegment& lhs, const LineSegment& rhs) {
    if (lhs.start.x < rhs.start.x) {
        return true;
    }
    if (lhs.start.x > rhs.start.x) {
        return false;
    }
    return lhs.start.y < rhs.start.y;
}

bool isPointInsidePolygon(const Point& point, const std::vector<LineSegment>& polygon) {
    int count = 0;
    for (const LineSegment& edge : polygon) {
        if ((edge.start.y > point.y) != (edge.end.y > point.y) &&
            (point.x < (edge.end.x - edge.start.x) * (point.y - edge.start.y) / (edge.end.y - edge.start.y) + edge.start.x)) {
            count++;
        }
    }
    return count % 2 == 1;
}


bool DotOnEdge(const Point& point, const LineSegment& edge) {
    double x1 = edge.start.x;
    double y1 = edge.start.y;
    double x2 = edge.end.x;
    double y2 = edge.end.y;

    // ���������, ��� ����� ����� �� �������, ��������� ��������������� ��������� �������.
    double crossProduct = (point.x - x1) * (y2 - y1) - (point.y - y1) * (x2 - x1);

    if (fabs(crossProduct) > std::numeric_limits<double>::epsilon()) {
        // ���� crossProduct �� ������ � ����, ����� �� ����� �� �������.
        return false;
    }

    // ����� ���������, ��� ����� ������ �������, �������� ���������� x � y.
    if (fabs(x1 - x2) > std::numeric_limits<double>::epsilon()) {
        return (x1 <= point.x && point.x <= x2) || (x2 <= point.x && point.x <= x1);
    }
    else {
        return (y1 <= point.y && point.y <= y2) || (y2 <= point.y && point.y <= y1);
    }
}

void drawPolygon(const std::vector<LineSegment>& polygon) {
    // ����� ������� ������
    double minX = polygon[0].start.x, minY = polygon[0].start.y, maxX = polygon[0].start.x, maxY = polygon[0].start.y;
    for (const LineSegment& edge : polygon) {
        if (edge.start.x < minX) minX = edge.start.x;
        if (edge.start.y < minY) minY = edge.start.y;
        if (edge.end.x < minX) minX = edge.end.x;
        if (edge.end.y < minY) minY = edge.end.y;

        if (edge.start.x > maxX) maxX = edge.start.x;
        if (edge.start.y > maxY) maxY = edge.start.y;
        if (edge.end.x > maxX) maxX = edge.end.x;
        if (edge.end.y > maxY) maxY = edge.end.y;
    }

    const int width = static_cast<int>(maxX - minX) + 1;
    const int height = static_cast<int>(maxY - minY) + 1;

    // ������� ��������� ������ ��� ��������� ������
    char** drawing = new char* [height];
    for (int i = 0; i < height; ++i) {
        drawing[i] = new char[width];
        for (int j = 0; j < width; ++j) {
            drawing[i][j] = ' ';
        }
    }

    // �������� ����� ������ � �������
    for (const LineSegment& edge : polygon) {
        int x1 = static_cast<int>(edge.start.x - minX);
        int y1 = static_cast<int>(edge.start.y - minY);
        int x2 = static_cast<int>(edge.end.x - minX);
        int y2 = static_cast<int> (edge.end.y - minY);

        // ��������� ����� � �������
        int dx = std::abs(x2 - x1);
        int dy = std::abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int error = dx - dy;

        while (true) {
            drawing[y1][x1] = '*';
            if (x1 == x2 && y1 == y2) break;
            int e2 = 2 * error;
            if (e2 > -dy) {
                error -= dy;
                x1 += sx;
            }
            if (e2 < dx) {
                error += dx;
                y1 += sy;
            }
        }
    }

    // ���������� ������ � �������
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            std::cout << drawing[i][j];
        }
        std::cout << std::endl;
    }

    // ���������� ������
    for (int i = 0; i < height; ++i) {
        delete[] drawing[i];
    }
    delete[] drawing;
}


void drawLines(const std::vector<Point>& points) {
    char sign = '.';
    if (points.size() == 0) { sign = '#'; }



    if (points.empty()) {
        std::cerr << "Error: The list of points is empty." << std::endl;
        return;
    }

    // ����� ������� ������
    double minX = points[0].x, minY = points[0].y, maxX = points[0].x, maxY = points[0].y;
    for (const Point& point : points) {
        if (point.x < minX) minX = point.x;
        if (point.y < minY) minY = point.y;
        if (point.x > maxX) maxX = point.x;
        if (point.y > maxY) maxY = point.y;
    }

    const int width = static_cast<int>(maxX - minX) + 1;
    const int height = static_cast<int>(maxY - minY) + 1;

    // ������� ��������� ������ ��� ��������� ������
    char** drawing = new char* [height];
    for (int i = 0; i < height; ++i) {
        drawing[i] = new char[width];
        for (int j = 0; j < width; ++j) {
            drawing[i][j] = ' ';
        }
    }

    // �������� ����� ����� ������� � �������
    for (size_t i = 1; i < points.size(); ++i) {
        const Point& point1 = points[i - 1];
        const Point& point2 = points[i];

        int x1 = static_cast<int>(point1.x - minX);
        int y1 = static_cast<int>(point1.y - minY);
        int x2 = static_cast<int>(point2.x - minX);
        int y2 = static_cast<int>(point2.y - minY);

        // ��������� �������� ����� � �������
        int dx = std::abs(x2 - x1);
        int dy = std::abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int error = dx - dy;

        while (true) {
            drawing[y1][x1] = sign; // ���������� ������ '#' ��� �������� �����
            if (x1 == x2 && y1 == y2) break;
            int e2 = 2 * error;
            if (e2 > -dy) {
                error -= dy;
                x1 += sx;
            }
            if (e2 < dx) {
                error += dx;
                y1 += sy;
            }
        }
    }

    // ���������� ������ � �������
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            std::cout << drawing[i][j];
        }
        std::cout << std::endl;
    }

    // ���������� ������
    for (int i = 0; i < height; ++i) {
        delete[] drawing[i];
    }
    delete[] drawing;
}

void drawPoint(const Point& point) {
    // ���������� ������� ������ ��� �������, � ������� �� ������ �������� �����.
    const int screenWidth = 80; // ������ ������
    const int screenHeight = 24; // ������ ������

    // ���������, ��� ���������� ����� ��������� � �������� ������.
    if (point.x >= 0 && point.x < screenWidth && point.y >= 0 && point.y < screenHeight) {
        // ���������� ����� �� ������, ��������, ��������� ������ 'X'.
        std::vector<std::vector<char>> screen(screenHeight, std::vector<char>(screenWidth, ' '));

        screen[point.y][point.x] = 'X'; // ���������� ������ 'X' �� ������ � �������� �����������.

        // �������� ���������� ������ � �������.
        for (int y = 0; y < screenHeight; ++y) {
            for (int x = 0; x < screenWidth; ++x) {
                std::cout << screen[y][x];
            }
            std::cout << std::endl;
        }
    }
}

