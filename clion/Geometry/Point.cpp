#include "stdafx.h"
#include "Point.h"

// Public methods

Point::Point()
{
	_x = DEFAULT_VALUE;
	_y = DEFAULT_VALUE;
}

Point::Point(double x, double y, bool polar)
{
	if (!polar)
	{
		_x = x; _y = y;
	} 
	else
	{
		// XXXXXXXX
	}
}

Point::Point(const Point& point)
{
	_x = point._x;
	_y = point._y;
}

Point::~Point()
{
}

Point::PointClassification Point::classify(Point & p0, Point & p1)
{
	// a = vector del segmento (p1 - p0)
	// b = vector desde el origen del segmento hasta el punto actual (this - p0)
	Point a(p1._x - p0._x, p1._y - p0._y);
	Point b(_x - p0._x, _y - p0._y);

	double sa = a._x * b._y - b._x * a._y;

	if (sa > 0.0)
		return PointClassification::LEFT;

	if (sa < 0.0)
		return PointClassification::RIGHT;

	// Si sa == 0

	if ((a._x * b._x < 0.0) || (a._y * b._y < 0.0))
		return PointClassification::BACKWARD;

	if (b.getModule() > a.getModule())
		return PointClassification::FORWARD;

	if (_x == p0._x && _y == p0._y)
		return PointClassification::ORIGIN;

	if (_x == p1._x && _y == p1._y)
		return PointClassification::DEST;

	return PointClassification::BETWEEN;
}

bool Point::colinear(Point& a, Point& b)
{
	PointClassification result = classify(a, b);
	return (result != PointClassification::LEFT) && (result != PointClassification::RIGHT);
}

double Point::distance(Point & p)
{
	double dx = p._x - _x;
	double dy = p._y - _y;
	return std::sqrt(dx * dx + dy * dy);
}

double Point::getAlpha()
{
	return std::atan2(_y, _x);
}

double Point::getModule()
{
	return std::sqrt(std::pow(_x, 2) + std::pow(_y, 2));
}

bool Point::leftAbove(Point& a, Point& b)
{
	PointClassification result = classify(a, b);
	return (result == PointClassification::LEFT) || (result != PointClassification::RIGHT);
}

Point & Point::operator=(const Point & point)
{
	_x = point._x;
	_y = point._y;

	return *this;
}

std::ostream& operator<<(std::ostream& os, const Point& point)
{
	os << "Coordinate X: " << point._x << ", coordinate Y: " << point._y;

	return os;
}

bool Point::rightAbove(Point& a, Point& b)
{
	PointClassification result = classify(a, b);
	return (result == PointClassification::RIGHT) || (result != PointClassification::LEFT);
}

double Point::slope(Point & p)
{
	double dx = p._x - _x;
	double dy = p._y - _y;

	// para evitar problemas de precisión.
	if (std::abs(dx) < 1e-9)
	{
		// línea VERTICAL.
		return std::numeric_limits<double>::infinity();
	}

	return dy / dx;
}

double Point::triangleArea2(Point& a, Point& b)
{
	return (_x * a.getY() - _y * a.getX() + a.getX() * b.getY() - a.getY() * b.getX() + b.getX() * _y - b.getY() * _x);
}
