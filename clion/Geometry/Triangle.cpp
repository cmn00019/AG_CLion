#include "stdafx.h"
#include "Triangle.h"

#include "Circle.h"

// Public methods

Triangle::Triangle(Vect2d &aa, Vect2d &bb, Vect2d &cc): _a(aa), _b(bb), _c(cc)
{
}

Triangle::Triangle(const Triangle &tt): _a(tt._a), _b(tt._b), _c(tt._c) 
{
}

Triangle::~Triangle()
{
}

double Triangle::area()
{
    double det = (_b.getX() - _a.getX()) * (_c.getY() - _a.getY()) -
                 (_b.getY() - _a.getY()) * (_c.getX() - _a.getX());

    return 0.5 * std::abs(det);
}

Circle Triangle::getInscribed() {
    double edgeA = _b.distance(_c);
    double edgeB = _c.distance(_a);
    double edgeC = _a.distance(_b);

    double perim = edgeA + edgeB + edgeC;
    double s = 0.5 * perim;

    double center_x = (edgeA * _a.getX() + edgeB * _b.getX() + edgeC * _c.getX()) / perim;
    double center_y = (edgeA * _a.getY() + edgeB * _b.getY() + edgeC * _c.getY()) / perim;

    double radius = area() / s;

    return Circle(Point(center_x, center_y), radius);
}

Circle Triangle::getCirumscribed() {
    double ax = _a.getX(), ay = _a.getY();
    double bx = _b.getX(), by = _b.getY();
    double cx = _c.getX(), cy = _c.getY();

    double D = 2.0 * (ax * (by - cy) + bx * (cy - ay) + cx * (ay - by));

    if (std::abs(D) < glm::epsilon<double>())
        return Circle(Point(ax, ay), 0.0);

    double ax2ay2 = ax * ax + ay * ay;
    double bx2by2 = bx * bx + by * by;
    double cx2cy2 = cx * cx + cy * cy;

    double center_x = (ax2ay2 * (by - cy) + bx2by2 * (cy - ay) + cx2cy2 * (ay - by)) / D;
    double center_y = (ax2ay2 * (cx - bx) + bx2by2 * (ax - cx) + cx2cy2 * (bx - ax)) / D;

    Point center(center_x, center_y);
    Point vertA(ax, ay);
    double radius = center.distance(vertA);

    return Circle(center, radius);
}