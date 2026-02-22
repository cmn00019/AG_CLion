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
    double xAB = _b.getX() - _a.getX(), yAB = _b.getY() - _a.getY();
    double xAC = _c.getX() - _a.getX(), yAC = _c.getY() - _a.getY();

    double b2 = xAC * xAC + yAC * yAC;  // |AC|^2
    double c2 = xAB * xAB + yAB * yAB;  // |AB|^2

    double areaTri = _a.triangleArea2(_b, _c);  // 2 * area con signo
    double den = 2.0 * areaTri;                  // 4 * AreaTri

    double xP = _a.getX() + (yAC * c2 - yAB * b2) / den;
    double yP = _a.getY() + (b2 * xAB - c2 * xAC) / den;

    double a = _b.distance(_c);
    double b = _a.distance(_c);
    double c = _a.distance(_b);
    double R = (a * b * c) / (2.0 * std::abs(areaTri));

    return Circle(Point(xP, yP), R);
}