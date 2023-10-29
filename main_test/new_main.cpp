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
    double incidentAngle; // Óãîë ïàäåíèÿ
    double reflectionAngle; // Óãîë îòðàæåíèÿ
    bool find;
    bool twice_more_visited;

};

bool operator<(const LineSegment& lhs, const LineSegment& rhs) {
    // Сравниваем начальные точки линий
    if (lhs.start.x < rhs.start.x) {
        return true;
    }
    if (lhs.start.x > rhs.start.x) {
        return false;
    }
    // Если x-координаты равны, сравниваем y-координаты
    return lhs.start.y < rhs.start.y;
}




struct LineSegment {
    Point start;
    Point end;
    LineSegment(const Point& start, const Point& end) : start(start), end(end) {}
};


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
    // Ïðîâåðêà, ÷òî startPoint íå ëåæèò íà edge
    if (startPoint == edge.start || startPoint == edge.end) {
        return result; // Íà÷àëüíàÿ òî÷êà ñîâïàäàåò ñ îäíîé èç êîíå÷íûõ òî÷åê, íåò ïåðåñå÷åíèÿ.
    }

    // Ïðîâåðêà, ÷òî startPoint íå ëåæèò íà îòðåçêå edge
    if (
        (startPoint.x > std::min(edge.start.x, edge.end.x) && startPoint.x < std::max(edge.start.x, edge.end.x)) &&
        (startPoint.y > std::min(edge.start.y, edge.end.y) && startPoint.y < std::max(edge.start.y, edge.end.y))
        ) {
        return result; // startPoint ëåæèò íà îòðåçêå edge, íåò ïåðåñå÷åíèÿ.
    }


    result.find = false;

    // Ïàðàìåòðû ëó÷à
    double x1 = startPoint.x;
    double y1 = startPoint.y;
    double x2 = x1 + cos(angle);
    double y2 = y1 + sin(angle);

    double x3 = edge.start.x;
    double y3 = edge.start.y;
    double x4 = edge.end.x;
    double y4 = edge.end.y;

    // Ïðîâåðÿåì, ïåðåñåêàåòñÿ ëè ëó÷ ñ îòðåçêîì
    double denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    if (denominator != 0) {
        double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denominator;
        double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denominator;



        //if (t >= 0 && t <= 1 && u > 0) {
        if (u > 0 && u <= 1 && t > 0) {
            // Ëèíèè ïåðåñåêàþòñÿ
            if (visited[edge] > 0) {

                result.intersectionPoint.x = x1 + t * (x2 - x1);
                result.intersectionPoint.y = y1 + t * (y2 - y1);

                double normalAngle = atan2(y4 - y3, x4 - x3);
                result.incidentAngle = normalAngle - angle;
                result.reflectionAngle = normalAngle + result.incidentAngle; // Óãîë îòðàæåíèÿ
                result.find = true;

                // Îãðàíè÷èâàåì óãîë â äèàïàçîíå [0, 2*Pi]
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
    if (reflect_q == polygon.size()) { return true; } // íàéäåí íóæíûé óãîë ïðè ïðîõîæäåíèè ñòîðîíû 1 ðàç

    for (const LineSegment& edge : polygon) {
        IntersectionResult result = calculateIntersection(startPoint, angle, edge, visited);
        if (result.twice_more_visited) { return false; } // åñëè ïåðåñå÷åíèé ñ 1 ðåáðîì áîëåå 1 ðàçà âûõîäèì èç öèêëà è ýòîò óãîë íå ïðîâåðÿåì 

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
    // Íàéòè ãðàíèöû ôèãóðû
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

    // Ñîçäàòü äâóìåðíûé ìàññèâ äëÿ îòðèñîâêè ôèãóðû
    char** drawing = new char* [height];
    for (int i = 0; i < height; ++i) {
        drawing[i] = new char[width];
        for (int j = 0; j < width; ++j) {
            drawing[i][j] = ' ';
        }
    }

    // Îòìåòèòü ëèíèè ôèãóðû â ìàññèâå
    for (const LineSegment& edge : polygon) {
        int x1 = static_cast<int>(edge.start.x - minX);
        int y1 = static_cast<int>(edge.start.y - minY);
        int x2 = static_cast<int>(edge.end.x - minX);
        int y2 = static_cast<int> (edge.end.y - minY);

        // Îòðèñîâêà ëèíèè â ìàññèâå
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

    // Îòîáðàçèòü ìàññèâ â êîíñîëè
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            std::cout << drawing[i][j];
        }
        std::cout << std::endl;
    }

    // Îñâîáîäèòü ïàìÿòü
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

    // Íàéòè ãðàíèöû ôèãóðû
    double minX = points[0].x, minY = points[0].y, maxX = points[0].x, maxY = points[0].y;
    for (const Point& point : points) {
        if (point.x < minX) minX = point.x;
        if (point.y < minY) minY = point.y;
        if (point.x > maxX) maxX = point.x;
        if (point.y > maxY) maxY = point.y;
    }

    const int width = static_cast<int>(maxX - minX) + 1;
    const int height = static_cast<int>(maxY - minY) + 1;

    // Ñîçäàòü äâóìåðíûé ìàññèâ äëÿ îòðèñîâêè ôèãóðû
    char** drawing = new char* [height];
    for (int i = 0; i < height; ++i) {
        drawing[i] = new char[width];
        for (int j = 0; j < width; ++j) {
            drawing[i][j] = ' ';
        }
    }

    // Îòìåòèòü ëèíèè ìåæäó òî÷êàìè â ìàññèâå
    for (size_t i = 1; i < points.size(); ++i) {
        const Point& point1 = points[i - 1];
        const Point& point2 = points[i];

        int x1 = static_cast<int>(point1.x - minX);
        int y1 = static_cast<int>(point1.y - minY);
        int x2 = static_cast<int>(point2.x - minX);
        int y2 = static_cast<int>(point2.y - minY);

        // Îòðèñîâêà ñïëîøíîé ëèíèè â ìàññèâå
        int dx = std::abs(x2 - x1);
        int dy = std::abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int error = dx - dy;

        while (true) {
            drawing[y1][x1] = '.'; // Èñïîëüçóåì ñèìâîë '#' äëÿ ñïëîøíîé ëèíèè
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

    // Îòîáðàçèòü ìàññèâ â êîíñîëè
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            std::cout << drawing[i][j];
        }
        std::cout << std::endl;
    }

    // Îñâîáîäèòü ïàìÿòü
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

    if (!isPointInsidePolygon(startPoint, polygon)) { // ýòî â íà÷àëî ïåðåíåñòè ïîòîì 
        std::cout << "start dot is out of figure" << std::endl;
        return 0;
    }

    double angleStep = M_PI / 1800.0; // Øàã óãëà â ðàäèàíàõ (1/10 ãðàäóñà)


    for (int i = 0; i < 3600; ++i) {
        double currentAngle = i * angleStep;
        double rayX = cos(currentAngle);
        double rayY = sin(currentAngle);
        // Òåïåðü ó âàñ åñòü ëó÷ ñ óãëîì currentAngle è íàïðàâëåíèåì (rayX, rayY)
        // Âû ìîæåòå èñïîëüçîâàòü åãî äëÿ ÷åãî óãîäíî, íàïðèìåð, äëÿ ïðîâåðêè ïåðåñå÷åíèé èëè äðóãèõ îïåðàöèé.
        std::cout << "Óãîë: " << currentAngle * 180.0 / M_PI << " ãðàäóñîâ, Íàïðàâëåíèå: (" << rayX << ", " << rayY << ")" << std::endl;

        //1. çàïóñêàþ èòåðàöèþ ïî ñòîðîíàì ìíîãîóãîëüíèêà
        int reflect_q = 0;
        std::map<LineSegment, int> visited;
        std::vector<Point> points;
        points.push_back(startPoint);
        if (findIntersection(startPoint, currentAngle, polygon, visited, reflect_q, points)) { std::cout << "Find ANGLE : " << currentAngle << endl; break; }

    }

    cout << "Not find need angle" << endl;

    return 0;
}

