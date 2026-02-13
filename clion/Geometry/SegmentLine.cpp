#include "stdafx.h"
#include "SegmentLine.h"

#include "Line.h"
#include "RayLine.h"

// Public methods

SegmentLine::SegmentLine()
{
}

SegmentLine::SegmentLine(const Point& a, const Point& b) : _orig(a), _dest(b)
{
}

SegmentLine::SegmentLine(const SegmentLine& segment) : _orig(segment._orig), _dest(segment._dest)
{
}

SegmentLine::SegmentLine(double ax, double ay, double bx, double by) : _orig(ax, ay), _dest(bx, by)
{
}

SegmentLine::~SegmentLine()
{
}

SegmentLine & SegmentLine::operator=(const SegmentLine & segment)
{
	if (this != &segment)
	{
		this->_orig = segment._orig;
		this->_dest = segment._dest;
	}

	return *this;
}

bool SegmentLine::segmentIntersection(SegmentLine& l)
{
	Point a = this->_orig;
	Point b = this->_dest;
	Point c = l._orig;
	Point d = l._dest;

	if (a.colinear(c,d) || b.colinear(c,d) ||
		c.colinear(a,b) || d.colinear(a,b))
		return false;
	return (a.left(c,d) ^ b.left(c,d) &&
		c.left(a,b) ^ d.left(a,b));
}

double SegmentLine::getC()
{
	if (isVertical())
		return std::numeric_limits<double>::infinity();

	double m = slope();

	return _orig.getY() - m * _orig.getX();
}

bool SegmentLine::distinct(SegmentLine & segment)
{
	return !equal(segment);
}

float SegmentLine::distPointSegment(Vect2d& vector)
{
	return 0.0f;
}

bool SegmentLine::equal(SegmentLine & segment)
{
	return (_orig.equal(segment._orig) && _dest.equal(segment._dest)) || (_orig.equal(segment._dest) && _dest.equal(segment._orig));
}

Point SegmentLine::getPoint(double t)
{
    double x = _orig.getX() + t * (_dest.getX() - _orig.getX());
	double y = _orig.getY() + t * (_dest.getY() - _orig.getY());

	return Point(x, y);
}

bool SegmentLine::impSegmentIntersection(SegmentLine& segment)
{
	Point a = this->_orig;
	Point b = this->_dest;
	Point c = segment._orig;
	Point d = segment._dest;

	Point::PointClassification cl;

	// 1. ¿Está C contenido en AB?
	cl = c.classify(a, b);
	if (cl == Point::PointClassification::BETWEEN ||
		cl == Point::PointClassification::ORIGIN ||
		cl == Point::PointClassification::DEST) return true;

	// 2. ¿Está D contenido en AB?
	cl = d.classify(a, b);
	if (cl == Point::PointClassification::BETWEEN ||
		cl == Point::PointClassification::ORIGIN ||
		cl == Point::PointClassification::DEST) return true;

	// 3. ¿Está A contenido en CD?
	cl = a.classify(c, d);
	if (cl == Point::PointClassification::BETWEEN ||
		cl == Point::PointClassification::ORIGIN ||
		cl == Point::PointClassification::DEST) return true;

	// 4. ¿Está B contenido en CD?
	cl = b.classify(c, d);
	if (cl == Point::PointClassification::BETWEEN ||
		cl == Point::PointClassification::ORIGIN ||
		cl == Point::PointClassification::DEST) return true;

	return false;
}

bool SegmentLine::isHorizontal()
{
	return BasicGeometry::equal(this->_orig.getY(), this->_dest.getY());
}

bool SegmentLine::isVertical()
{
	return BasicGeometry::equal(this->_orig.getX(), this->_dest.getX());
}

double SegmentLine::slope()
{
	if (isVertical())
		return std::numeric_limits<double>::infinity();

	double dx = _dest.getX() - _orig.getX();
	double dy = _dest.getY() - _orig.getY();

	return dy / dx;
}

// Protected methods

std::ostream& operator<<(std::ostream& os, const SegmentLine& segment)
{
	os << "Point A: " << segment._orig << ", Point B: " << segment._dest << "\n";

	return os;
}

// Protected methods

float SegmentLine::getDistanceT0(Vect2d& point)
{
	return 0.0f;
}