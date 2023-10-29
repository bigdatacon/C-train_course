#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>
#include <map>


using namespace std;


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
    double incidentAngle; // Г“ГЈГ®Г« ГЇГ Г¤ГҐГ­ГЁГї
    double reflectionAngle; // Г“ГЈГ®Г« Г®ГІГ°Г Г¦ГҐГ­ГЁГї
    bool find;
    bool twice_more_visited;

};

struct LineSegment {
    Point start;
    Point end;
    LineSegment(const Point& start, const Point& end) : start(start), end(end) {}
};


bool operator<(const LineSegment& lhs, const LineSegment& rhs) {
    // РЎСЂР°РІРЅРёРІР°РµРј РЅР°С‡Р°Р»СЊРЅС‹Рµ С‚РѕС‡РєРё Р»РёРЅРёР№
    if (lhs.start.x < rhs.start.x) {
        return true;
    }
    if (lhs.start.x > rhs.start.x) {
        return false;
    }
    // Р•СЃР»Рё x-РєРѕРѕСЂРґРёРЅР°С‚С‹ СЂР°РІРЅС‹, СЃСЂР°РІРЅРёРІР°РµРј y-РєРѕРѕСЂРґРёРЅР°С‚С‹
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

IntersectionResult calculateIntersection(const Point& startPoint, double angle, const LineSegment& edge, std::map<LineSegment, int>& visited) {
    IntersectionResult result;
    // ГЏГ°Г®ГўГҐГ°ГЄГ , Г·ГІГ® startPoint Г­ГҐ Г«ГҐГ¦ГЁГІ Г­Г  edge
    if (startPoint == edge.start || startPoint == edge.end) {
        return result; // ГЌГ Г·Г Г«ГјГ­Г Гї ГІГ®Г·ГЄГ  Г±Г®ГўГЇГ Г¤Г ГҐГІ Г± Г®Г¤Г­Г®Г© ГЁГ§ ГЄГ®Г­ГҐГ·Г­Г»Гµ ГІГ®Г·ГҐГЄ, Г­ГҐГІ ГЇГҐГ°ГҐГ±ГҐГ·ГҐГ­ГЁГї.
    }

    // ГЏГ°Г®ГўГҐГ°ГЄГ , Г·ГІГ® startPoint Г­ГҐ Г«ГҐГ¦ГЁГІ Г­Г  Г®ГІГ°ГҐГ§ГЄГҐ edge
    if (
        (startPoint.x > std::min(edge.start.x, edge.end.x) && startPoint.x < std::max(edge.start.x, edge.end.x)) &&
        (startPoint.y > std::min(edge.start.y, edge.end.y) && startPoint.y < std::max(edge.start.y, edge.end.y))
        ) {
        return result; // startPoint Г«ГҐГ¦ГЁГІ Г­Г  Г®ГІГ°ГҐГ§ГЄГҐ edge, Г­ГҐГІ ГЇГҐГ°ГҐГ±ГҐГ·ГҐГ­ГЁГї.
    }


    result.find = false;

    // ГЏГ Г°Г Г¬ГҐГІГ°Г» Г«ГіГ·Г 
    double x1 = startPoint.x;
    double y1 = startPoint.y;
    double x2 = x1 + cos(angle);
    double y2 = y1 + sin(angle);

    double x3 = edge.start.x;
    double y3 = edge.start.y;
    double x4 = edge.end.x;
    double y4 = edge.end.y;

    // ГЏГ°Г®ГўГҐГ°ГїГҐГ¬, ГЇГҐГ°ГҐГ±ГҐГЄГ ГҐГІГ±Гї Г«ГЁ Г«ГіГ· Г± Г®ГІГ°ГҐГ§ГЄГ®Г¬
    double denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    if (denominator != 0) {
        double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denominator;
        double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denominator;



        //if (t >= 0 && t <= 1 && u > 0) {
        if (u > 0 && u <= 1 && t > 0) {
            // Г‹ГЁГ­ГЁГЁ ГЇГҐГ°ГҐГ±ГҐГЄГ ГѕГІГ±Гї
            if (visited[edge] > 0) {

                result.intersectionPoint.x = x1 + t * (x2 - x1);
                result.intersectionPoint.y = y1 + t * (y2 - y1);

                double normalAngle = atan2(y4 - y3, x4 - x3);
                result.incidentAngle = normalAngle - angle;
                result.reflectionAngle = normalAngle + result.incidentAngle; // Г“ГЈГ®Г« Г®ГІГ°Г Г¦ГҐГ­ГЁГї
                result.find = true;

                // ГЋГЈГ°Г Г­ГЁГ·ГЁГўГ ГҐГ¬ ГіГЈГ®Г« Гў Г¤ГЁГ ГЇГ Г§Г®Г­ГҐ [0, 2*Pi]
                while (result.reflectionAngle < 0) {
                    result.reflectionAngle += 2 * M_PI;
                }
                while (result.reflectionAngle >= 2 * M_PI) {
                    result.reflectionAngle -= 2 * M_PI;
                }
                visited[edge]++;

            }
            else { result.twice_more_visited = true; return result; }
        }
    }

    return result;
}


bool findIntersection(const Point& startPoint, double angle, const std::vector<LineSegment>& polygon, std::map<LineSegment, int> visited, int& reflect_q, std::vector<Point>& points) {
    if (reflect_q == polygon.size()) { return true; } // Г­Г Г©Г¤ГҐГ­ Г­ГіГ¦Г­Г»Г© ГіГЈГ®Г« ГЇГ°ГЁ ГЇГ°Г®ГµГ®Г¦Г¤ГҐГ­ГЁГЁ Г±ГІГ®Г°Г®Г­Г» 1 Г°Г Г§

    for (const LineSegment& edge : polygon) {
        IntersectionResult result = calculateIntersection(startPoint, angle, edge, visited);
        if (result.twice_more_visited) { return false; } // ГҐГ±Г«ГЁ ГЇГҐГ°ГҐГ±ГҐГ·ГҐГ­ГЁГ© Г± 1 Г°ГҐГЎГ°Г®Г¬ ГЎГ®Г«ГҐГҐ 1 Г°Г Г§Г  ГўГ»ГµГ®Г¤ГЁГ¬ ГЁГ§ Г¶ГЁГЄГ«Г  ГЁ ГЅГІГ®ГІ ГіГЈГ®Г« Г­ГҐ ГЇГ°Г®ГўГҐГ°ГїГҐГ¬ 

        if (result.find) {
            Point next_point(result.intersectionPoint.x, result.intersectionPoint.y);
            double reflectionAngle = result.reflectionAngle;
            reflect_q++;
            points.push_back(next_point);
            return findIntersection(next_point, reflectionAngle, polygon, visited, reflect_q, points);
        }

    }
    return false;
}


void drawPolygon(const std::vector<LineSegment>& polygon) {
    // ГЌГ Г©ГІГЁ ГЈГ°Г Г­ГЁГ¶Г» ГґГЁГЈГіГ°Г»
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

    // Г‘Г®Г§Г¤Г ГІГј Г¤ГўГіГ¬ГҐГ°Г­Г»Г© Г¬Г Г±Г±ГЁГў Г¤Г«Гї Г®ГІГ°ГЁГ±Г®ГўГЄГЁ ГґГЁГЈГіГ°Г»
    char** drawing = new char* [height];
    for (int i = 0; i < height; ++i) {
        drawing[i] = new char[width];
        for (int j = 0; j < width; ++j) {
            drawing[i][j] = ' ';
        }
    }

    // ГЋГІГ¬ГҐГІГЁГІГј Г«ГЁГ­ГЁГЁ ГґГЁГЈГіГ°Г» Гў Г¬Г Г±Г±ГЁГўГҐ
    for (const LineSegment& edge : polygon) {
        int x1 = static_cast<int>(edge.start.x - minX);
        int y1 = static_cast<int>(edge.start.y - minY);
        int x2 = static_cast<int>(edge.end.x - minX);
        int y2 = static_cast<int> (edge.end.y - minY);

        // ГЋГІГ°ГЁГ±Г®ГўГЄГ  Г«ГЁГ­ГЁГЁ Гў Г¬Г Г±Г±ГЁГўГҐ
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

    // ГЋГІГ®ГЎГ°Г Г§ГЁГІГј Г¬Г Г±Г±ГЁГў Гў ГЄГ®Г­Г±Г®Г«ГЁ
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            std::cout << drawing[i][j];
        }
        std::cout << std::endl;
    }

    // ГЋГ±ГўГ®ГЎГ®Г¤ГЁГІГј ГЇГ Г¬ГїГІГј
    for (int i = 0; i < height; ++i) {
        delete[] drawing[i];
    }
    delete[] drawing;
}


void drawLines(const std::vector<Point>& points) {
    if (points.empty()) {
        std::cerr << "Error: The list of points is empty." << std::endl;
        return;
    }

    // ГЌГ Г©ГІГЁ ГЈГ°Г Г­ГЁГ¶Г» ГґГЁГЈГіГ°Г»
    double minX = points[0].x, minY = points[0].y, maxX = points[0].x, maxY = points[0].y;
    for (const Point& point : points) {
        if (point.x < minX) minX = point.x;
        if (point.y < minY) minY = point.y;
        if (point.x > maxX) maxX = point.x;
        if (point.y > maxY) maxY = point.y;
    }

    const int width = static_cast<int>(maxX - minX) + 1;
    const int height = static_cast<int>(maxY - minY) + 1;

    // Г‘Г®Г§Г¤Г ГІГј Г¤ГўГіГ¬ГҐГ°Г­Г»Г© Г¬Г Г±Г±ГЁГў Г¤Г«Гї Г®ГІГ°ГЁГ±Г®ГўГЄГЁ ГґГЁГЈГіГ°Г»
    char** drawing = new char* [height];
    for (int i = 0; i < height; ++i) {
        drawing[i] = new char[width];
        for (int j = 0; j < width; ++j) {
            drawing[i][j] = ' ';
        }
    }

    // ГЋГІГ¬ГҐГІГЁГІГј Г«ГЁГ­ГЁГЁ Г¬ГҐГ¦Г¤Гі ГІГ®Г·ГЄГ Г¬ГЁ Гў Г¬Г Г±Г±ГЁГўГҐ
    for (size_t i = 1; i < points.size(); ++i) {
        const Point& point1 = points[i - 1];
        const Point& point2 = points[i];

        int x1 = static_cast<int>(point1.x - minX);
        int y1 = static_cast<int>(point1.y - minY);
        int x2 = static_cast<int>(point2.x - minX);
        int y2 = static_cast<int>(point2.y - minY);

        // ГЋГІГ°ГЁГ±Г®ГўГЄГ  Г±ГЇГ«Г®ГёГ­Г®Г© Г«ГЁГ­ГЁГЁ Гў Г¬Г Г±Г±ГЁГўГҐ
        int dx = std::abs(x2 - x1);
        int dy = std::abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int error = dx - dy;

        while (true) {
            drawing[y1][x1] = '.'; // Г€Г±ГЇГ®Г«ГјГ§ГіГҐГ¬ Г±ГЁГ¬ГўГ®Г« '#' Г¤Г«Гї Г±ГЇГ«Г®ГёГ­Г®Г© Г«ГЁГ­ГЁГЁ
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

    // ГЋГІГ®ГЎГ°Г Г§ГЁГІГј Г¬Г Г±Г±ГЁГў Гў ГЄГ®Г­Г±Г®Г«ГЁ
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            std::cout << drawing[i][j];
        }
        std::cout << std::endl;
    }

    // ГЋГ±ГўГ®ГЎГ®Г¤ГЁГІГј ГЇГ Г¬ГїГІГј
    for (int i = 0; i < height; ++i) {
        delete[] drawing[i];
    }
    delete[] drawing;
}

int main() {
    std::vector<LineSegment> polygon;
    std::vector<Point> test_line;
    polygon.push_back(LineSegment(Point(1.0, 1.0), Point(7.0, 1.0)));
    polygon.push_back(LineSegment(Point(7.0, 1.0), Point(7.0, 7.0)));
    polygon.push_back(LineSegment(Point(7.0, 7.0), Point(1.0, 7.0)));
    polygon.push_back(LineSegment(Point(1.0, 7.0), Point(1.0, 1.0)));

    Point startPoint(2.0, 2.0);

    if (!isPointInsidePolygon(startPoint, polygon)) { // ГЅГІГ® Гў Г­Г Г·Г Г«Г® ГЇГҐГ°ГҐГ­ГҐГ±ГІГЁ ГЇГ®ГІГ®Г¬ 
        std::cout << "start dot is out of figure" << std::endl;
        return 0;
    }

    double angleStep = M_PI / 1800.0; // ГГ ГЈ ГіГЈГ«Г  Гў Г°Г Г¤ГЁГ Г­Г Гµ (1/10 ГЈГ°Г Г¤ГіГ±Г )


    for (int i = 0; i < 3600; ++i) {
        double currentAngle = i * angleStep;
        double rayX = cos(currentAngle);
        double rayY = sin(currentAngle);
        // Г’ГҐГЇГҐГ°Гј Гі ГўГ Г± ГҐГ±ГІГј Г«ГіГ· Г± ГіГЈГ«Г®Г¬ currentAngle ГЁ Г­Г ГЇГ°Г ГўГ«ГҐГ­ГЁГҐГ¬ (rayX, rayY)
        // Г‚Г» Г¬Г®Г¦ГҐГІГҐ ГЁГ±ГЇГ®Г«ГјГ§Г®ГўГ ГІГј ГҐГЈГ® Г¤Г«Гї Г·ГҐГЈГ® ГіГЈГ®Г¤Г­Г®, Г­Г ГЇГ°ГЁГ¬ГҐГ°, Г¤Г«Гї ГЇГ°Г®ГўГҐГ°ГЄГЁ ГЇГҐГ°ГҐГ±ГҐГ·ГҐГ­ГЁГ© ГЁГ«ГЁ Г¤Г°ГіГЈГЁГµ Г®ГЇГҐГ°Г Г¶ГЁГ©.
        std::cout << "Г“ГЈГ®Г«: " << currentAngle * 180.0 / M_PI << " ГЈГ°Г Г¤ГіГ±Г®Гў, ГЌГ ГЇГ°Г ГўГ«ГҐГ­ГЁГҐ: (" << rayX << ", " << rayY << ")" << std::endl;

        //1. Г§Г ГЇГіГ±ГЄГ Гѕ ГЁГІГҐГ°Г Г¶ГЁГѕ ГЇГ® Г±ГІГ®Г°Г®Г­Г Г¬ Г¬Г­Г®ГЈГ®ГіГЈГ®Г«ГјГ­ГЁГЄГ 
        int reflect_q = 0;
        std::map<LineSegment, int> visited;
        std::vector<Point> points;
        points.push_back(startPoint);
        if (findIntersection(startPoint, currentAngle, polygon, visited, reflect_q, points)) { std::cout << "Find ANGLE : " << currentAngle << endl; break; }

    }

    cout << "Not find need angle" << endl;

    return 0;
}

