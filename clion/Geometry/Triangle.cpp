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

    double s = 0.5 * (edgeA + edgeB + edgeC);

    double center_x = (edgeA * _a.getX() + edgeB * _b.getX() + edgeC * _c.getX()) / 2*s;
    double center_y = (edgeA * _a.getY() + edgeB * _b.getY() + edgeC * _c.getY()) / 2*s;

    double radius = area() / s;

    return Circle(Point(center_x, center_y), radius);
}

Circle Triangle::getCirumscribed() {
    double edgeA = _b.distance(_c);
    double edgeB = _c.distance(_a);
    double edgeC = _a.distance(_b);

    double yAC = _c.getY() - _a.getY();
    double xAC = _c.getX() - _a.getX();
    double yAB = _b.getY() - _a.getY();
    double xAB = _b.getX() - _a.getX();

    double center_x = _a.getX() + ((yAC * edgeC * edgeC) - (edgeB * edgeB * yAB)) / 4 * area();
    double center_y = _a.getY() + ((edgeB * edgeB * xAB) - (xAC * edgeC * edgeC)) / 4 * area();

    double radius = (edgeA * edgeB * edgeC) / 4 * area();

    return Circle(Point(center_x, center_y), radius);
}