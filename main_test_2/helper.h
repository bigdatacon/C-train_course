

#pragma once

#include <vector>

struct Point {
    Point();
    Point(double x, double y);
    double x;
    double y;
};

bool operator==(const Point& lhs, const Point& rhs);
bool operator!=(const Point& lhs, const Point& rhs);

struct IntersectionResult {
    Point intersectionPoint;
    double incidentAngle;
    double reflectionAngle;
    bool find=false;
    bool twice_more_visited=false;
};

struct LineSegment {
    Point start;
    Point end;
    LineSegment(const Point& start, const Point& end);
};

bool operator<(const LineSegment& lhs, const LineSegment& rhs);

bool isPointInsidePolygon(const Point& point, const std::vector<LineSegment>& polygon);
bool DotOnEdge(const Point& point, const LineSegment& edge);

void drawPolygon(const std::vector<LineSegment>& polygon);
void drawLines(const std::vector<Point>& points);
void drawPoint(const Point& point);
