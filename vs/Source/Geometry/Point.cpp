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
	// XXXXX
	return PointClassification::BETWEEN;
}

bool Point::colinear(Point& a, Point& b)
{
	PointClassification result = classify(a, b);
	return (result != PointClassification::LEFT) && (result != PointClassification::RIGHT);
}

double Point::distance(Point & p)
{
	// XXXXX
	return .0;
}

double Point::getAlpha()
{
	// XXXXX
	// return angle;
	return 0; 
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
	// XXXXX
    return 0;
}

double Point::triangleArea2(Point& a, Point& b)
{
	// XXXXX
	return 0.0;
}
