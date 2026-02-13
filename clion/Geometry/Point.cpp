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
	Point a(p1._x - p0._x, p1._y - p0._y);
	Point b(_x - p0._x, _y - p0._y);

	// Calculamos el producto cruzado en 2D (determinante) para ver la orientación
	double sa = a._x * b._y - b._x * a._y;

	if (sa > 0.0)
		return PointClassification::LEFT;

	if (sa < 0.0)
		return PointClassification::RIGHT;

	// 1. Comprobamos si está "DETRÁS" (BACKWARD)
	if ((a._x * b._x < 0.0) || (a._y * b._y < 0.0))
		return PointClassification::BACKWARD;

	// 2. Comprobamos si está "DELANTE" (FORWARD)
	if (b.getModule() > a.getModule())
		return PointClassification::FORWARD;

	// 3. Comprobamos si es el "ORIGEN"
	if (_x == p0._x && _y == p0._y)
		return PointClassification::ORIGIN;

	// 4. Comprobamos si es el "DESTINO"
	if (_x == p1._x && _y == p1._y)
		return PointClassification::DEST;

	// 5. Si no es nada de lo anterior, está "EN MEDIO" (BETWEEN)
	return PointClassification::BETWEEN;
}

bool Point::colinear(Point& a, Point& b)
{
	PointClassification result = classify(a, b);
	return (result != PointClassification::LEFT) && (result != PointClassification::RIGHT);
}

double Point::distance(Point & p)
{
	return std::sqrt(std::pow(p._x - this->_x, 2) + std::pow(p._y - this->_y,2));
}

double Point::getAlpha()
{
	return std::atan2(_y,_x);
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
	double dx = p._x - this->_x;
	double dy = p._y - this->_y;

	if (std::abs(dx) < 1e-9)
	{
		return std::numeric_limits<double>::infinity();
	}

	return dy / dx;
}

double Point::triangleArea2(Point& a, Point& b)
{
	return (_x * a._y - _y * a._x + a._x * b._y -
			a._y * b._x + b._x * _y - b._y * _x);
}
