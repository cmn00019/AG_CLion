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
	// XXXXX
	return 0;
}

Vect2d & Vect2d::operator=(const Vect2d & vector)
{
	Point::operator=(vector);

	return *this;
}

Vect2d Vect2d::operator+(const Vect2d& b)
{
	// XXXXX
	return Vect2d();
}

Vect2d Vect2d::operator-(const Vect2d& b)
{
	// XXXXX
	return Vect2d();
}

Vect2d Vect2d::scalarMult(double t)
{
	// XXXXX
	return Vect2d();
}