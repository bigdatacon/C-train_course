#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include "helper.h"

using namespace std;

/*
struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};



bool operator==(const Point& lhs, const Point& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator!=(const Point& lhs, const Point& rhs) {
    return !(lhs == rhs);
}
struct IntersectionResult {
    Point intersectionPoint;
    double incidentAngle; // Угол падения
    double reflectionAngle; // Угол отражения
    bool find = false;
    bool twice_more_visited=false;

};


struct LineSegment {
    Point start;
    Point end;
    LineSegment(const Point& start, const Point& end) : start(start), end(end) {}
};

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

    // Проверяем, что точка лежит на отрезке, используя параметрическое уравнение отрезка.
    double crossProduct = (point.x - x1) * (y2 - y1) - (point.y - y1) * (x2 - x1);

    if (fabs(crossProduct) > std::numeric_limits<double>::epsilon()) {
        // Если crossProduct не близок к нулю, точка не лежит на отрезке.
        return false;
    }

    // Далее проверяем, что точка внутри отрезка, учитывая координаты x и y.
    if (fabs(x1 - x2) > std::numeric_limits<double>::epsilon()) {
        return (x1 <= point.x && point.x <= x2) || (x2 <= point.x && point.x <= x1);
    }
    else {
        return (y1 <= point.y && point.y <= y2) || (y2 <= point.y && point.y <= y1);
    }
}


*/

IntersectionResult calculateIntersection(const Point& startPoint, double angle, const LineSegment& edge, std::map<LineSegment, int>& visited) {
    IntersectionResult result;
    if (DotOnEdge(startPoint, edge)) { return result; } // Начальная точка лежит на отрезке}
        result.find = false;

    // Параметры луча
    double x1 = startPoint.x;
    double y1 = startPoint.y;
    double x2 = x1 + cos(angle);
    double y2 = y1 + sin(angle);

    double x3 = edge.start.x;
    double y3 = edge.start.y;
    double x4 = edge.end.x;
    double y4 = edge.end.y;

    // Проверяем, пересекается ли луч с отрезком
    double denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    if (denominator != 0) {
        double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denominator;
        double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denominator;



        //if (t >= 0 && t <= 1 && u > 0) {
        if (u > 0 && u <= 1 && t > 0) {
            // Линии пересекаются
            if (visited[edge] == 0) { //не было до этого пересечений


                double new_x = x1 + t * (x2 - x1);
                double new_y = y1 + t * (y2 - y1);

                if (!DotOnEdge(Point(new_x, new_y), edge)) { return result; } //точка пересечения не на отрезке

                result.intersectionPoint.x = new_x;
                result.intersectionPoint.y = new_y;

                double normalAngle = atan2(y4 - y3, x4 - x3);
                result.incidentAngle = normalAngle - angle;
                result.reflectionAngle = normalAngle + result.incidentAngle; // Угол отражения
                result.find = true;

                // Ограничиваем угол в диапазоне [0, 2*Pi]
                while (result.reflectionAngle < 0) {
                    result.reflectionAngle += 2 * M_PI;
                }
                while (result.reflectionAngle >= 2 * M_PI) {
                    result.reflectionAngle -= 2 * M_PI;
                }
                visited[edge]++;

            }
            // уже было пересечение с этим реброи
            else { result.twice_more_visited = true; return result; }
        }
    }

    return result;
}

//void drawLines(const std::vector<Point>& points);

bool findIntersection(const Point& startPoint, double angle, const std::vector<LineSegment>& polygon, std::map<LineSegment, int> visited, int& reflect_q, std::vector<Point>& points) {
    if (reflect_q == polygon.size()) {drawLines(points); return true; } // найден нужный угол при прохождении стороны 1 раз

    for (const LineSegment& edge : polygon) {
        IntersectionResult result = calculateIntersection(startPoint, angle, edge, visited);
        if (result.twice_more_visited) { return false; } // если пересечений с 1 ребром более 1 раза выходим из цикла и этот угол не проверяем 

        if (result.find) {
            Point next_point(result.intersectionPoint.x, result.intersectionPoint.y);
            double reflectionAngle = result.reflectionAngle;
            reflect_q++;
            points.push_back(next_point);
            //drawLines(points); // отрисовываю линию для отладки
            //std::this_thread::sleep_for(std::chrono::milliseconds(10000)); // небольшая пауза для отладки 
            return findIntersection(next_point, reflectionAngle, polygon, visited, reflect_q, points); // рекурсивно иду дальше от новой точки пересечения ищу следующую точку
        }

    }
    return false;
}

/*

void drawPolygon(const std::vector<LineSegment>& polygon) {
    // Найти границы фигуры
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

    // Создать двумерный массив для отрисовки фигуры
    char** drawing = new char* [height];
    for (int i = 0; i < height; ++i) {
        drawing[i] = new char[width];
        for (int j = 0; j < width; ++j) {
            drawing[i][j] = ' ';
        }
    }

    // Отметить линии фигуры в массиве
    for (const LineSegment& edge : polygon) {
        int x1 = static_cast<int>(edge.start.x - minX);
        int y1 = static_cast<int>(edge.start.y - minY);
        int x2 = static_cast<int>(edge.end.x - minX);
        int y2 = static_cast<int> (edge.end.y - minY);

        // Отрисовка линии в массиве
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

    // Отобразить массив в консоли
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            std::cout << drawing[i][j];
        }
        std::cout << std::endl;
    }

    // Освободить память
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

    // Найти границы фигуры
    double minX = points[0].x, minY = points[0].y, maxX = points[0].x, maxY = points[0].y;
    for (const Point& point : points) {
        if (point.x < minX) minX = point.x;
        if (point.y < minY) minY = point.y;
        if (point.x > maxX) maxX = point.x;
        if (point.y > maxY) maxY = point.y;
    }

    const int width = static_cast<int>(maxX - minX) + 1;
    const int height = static_cast<int>(maxY - minY) + 1;

    // Создать двумерный массив для отрисовки фигуры
    char** drawing = new char* [height];
    for (int i = 0; i < height; ++i) {
        drawing[i] = new char[width];
        for (int j = 0; j < width; ++j) {
            drawing[i][j] = ' ';
        }
    }

    // Отметить линии между точками в массиве
    for (size_t i = 1; i < points.size(); ++i) {
        const Point& point1 = points[i - 1];
        const Point& point2 = points[i];

        int x1 = static_cast<int>(point1.x - minX);
        int y1 = static_cast<int>(point1.y - minY);
        int x2 = static_cast<int>(point2.x - minX);
        int y2 = static_cast<int>(point2.y - minY);

        // Отрисовка сплошной линии в массиве
        int dx = std::abs(x2 - x1);
        int dy = std::abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int error = dx - dy;

        while (true) {
            drawing[y1][x1] = sign; // Используем символ '#' для сплошной линии
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

    // Отобразить массив в консоли
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            std::cout << drawing[i][j];
        }
        std::cout << std::endl;
    }

    // Освободить память
    for (int i = 0; i < height; ++i) {
        delete[] drawing[i];
    }
    delete[] drawing;
}

void drawPoint(const Point& point) {
    // Определите размеры экрана или область, в которой вы хотите рисовать точку.
    const int screenWidth = 80; // Ширина экрана
    const int screenHeight = 24; // Высота экрана

    // Проверьте, что координаты точки находятся в пределах экрана.
    if (point.x >= 0 && point.x < screenWidth && point.y >= 0 && point.y < screenHeight) {
        // Отобразите точку на экране, например, используя символ 'X'.
        std::vector<std::vector<char>> screen(screenHeight, std::vector<char>(screenWidth, ' '));

        screen[point.y][point.x] = 'X'; // Разместите символ 'X' на экране в заданных координатах.

        // Выведите содержимое экрана в консоль.
        for (int y = 0; y < screenHeight; ++y) {
            for (int x = 0; x < screenWidth; ++x) {
                std::cout << screen[y][x];
            }
            std::cout << std::endl;
        }
    }
}
*/
int main() {
    std::vector<LineSegment> polygon;
    std::vector<Point> test_line;
    /*
    polygon.push_back(LineSegment(Point(1.0, 1.0), Point(7.0, 1.0)));
    polygon.push_back(LineSegment(Point(7.0, 1.0), Point(7.0, 7.0)));
    polygon.push_back(LineSegment(Point(7.0, 7.0), Point(1.0, 7.0)));
    polygon.push_back(LineSegment(Point(1.0, 7.0), Point(1.0, 1.0)));

    Point startPoint(2.0, 2.0);
    */

    
    polygon.push_back(LineSegment(Point(2.0, 2.0), Point(4.0, 1.0)));
    polygon.push_back(LineSegment(Point(4.0, 1.0), Point(6.0, 2.0)));
    polygon.push_back(LineSegment(Point(6.0, 2.0), Point(6.0, 4.0)));
    polygon.push_back(LineSegment(Point(6.0, 4.0), Point(4.0, 6.0)));
    polygon.push_back(LineSegment(Point(4.0, 6.0), Point(2.0, 5.0)));
    polygon.push_back(LineSegment(Point(2.0, 5.0), Point(1.0, 3.0)));
    polygon.push_back(LineSegment(Point(1.0, 3.0), Point(2.0, 2.0)));

    Point startPoint(3.0, 3.0);
    


    if (!isPointInsidePolygon(startPoint, polygon)) { // это в начало перенести потом 
        std::cout << "start dot is out of figure" << std::endl;
        return 0;
    }

    double angleStep = M_PI / 1800.0; // Шаг угла в радианах (1/10 градуса)
    drawPolygon(polygon);

    std::vector<Point> test_line_start_point;
    test_line_start_point.push_back(startPoint);
    //drawLines(test_line_start_point); //рисую точку для проверки через вектор точек 
    drawPoint(startPoint);  // рисую точку для проверки через другую функцию 



    // Для быстрой отладке по уже найденному углу
    /*
    const double angleInDegrees = 33;  // Угол в градусах
    const double angleInRadians = angleInDegrees * M_PI / 180.0;  // Преобразование в радианы
    int reflect_q = 0;
    std::map<LineSegment, int> visited;
    std::vector<Point> points;
    points.push_back(startPoint);
    if (findIntersection(startPoint, angleInRadians, polygon, visited, reflect_q, points)) {
        std::cout << "Find ANGLE : " << angleInDegrees << endl;

        //drawLines(points);
        return 0;
    }
    else

    {
    

    cout << "Not find need angle" << endl;
    }
    
    */
    
    
    
    for (int i = 0; i < 3600; ++i) {
        double currentAngle = i * angleStep;

        //1. запускаю итерацию по сторонам многоугольника
        int reflect_q = 0;
        std::map<LineSegment, int> visited;
        std::vector<Point> points;
        points.push_back(startPoint);
        if (findIntersection(startPoint, currentAngle, polygon, visited, reflect_q, points)) { std::cout << "Find ANGLE : " << currentAngle * 180.0 / M_PI << "  DEGREE" << endl;
        
        drawLines(points);
        return 0;
        }


    }

    cout << "Not find need angle" << endl;
     
     
    return 0;
}

