#include "stdafx.h"
#include "Vect2d.h"

// Public methods

Vect2d::Vect2d(const Point& point) : Point(point)
{
}

Vect2d::Vect2d(const Vect2d& v) : Point(v._x, v._y)
{
}

Vect2d::~Vect2d()
{
}

double Vect2d::dot(Vect2d& b)
{
	return _x * b._x + _y * b._y;
}

Vect2d & Vect2d::operator=(const Vect2d & vector)
{
	Point::operator=(vector);

	return *this;
}

Vect2d Vect2d::operator+(const Vect2d& b)
{
	return Vect2d(_x + b._x, _y + b._y);
}

Vect2d Vect2d::operator-(const Vect2d& b)
{
	return Vect2d(_x - b._x, _y - b._y);
}

Vect2d Vect2d::scalarMult(double t)
{
	return Vect2d(_x * t, _y * t);
}