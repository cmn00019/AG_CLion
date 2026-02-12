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
	double area1 = l._orig.triangleArea2(_orig, _dest);
	double area2 = l._dest.triangleArea2(_orig, _dest);

	if (area1 * area2 >= 0)
		return false;

	double area3 = _orig.triangleArea2(l._orig, l._dest);
	double area4 = _dest.triangleArea2(l._orig, l._dest);

	if (area3 * area4 >= 0)
		return false;

	return true;
}

double SegmentLine::getC()
{
	double m = slope();
	if (std::isinf(m))
		return INFINITY;
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
	if (segment._orig.isBetween(_orig, _dest))
		return true;
	if (segment._dest.isBetween(_orig, _dest))
		return true;
	if (_orig.isBetween(segment._orig, segment._dest))
		return true;
	if (_dest.isBetween(segment._orig, segment._dest))
		return true;
	return false;
}

bool SegmentLine::isHorizontal()
{
	return BasicGeometry::equal(_orig.getY(), _dest.getY());
}

bool SegmentLine::isVertical()
{
	return BasicGeometry::equal(_orig.getX(), _dest.getX());
}

double SegmentLine::slope()
{
	double dx = _dest.getX() - _orig.getX();
	double dy = _dest.getY() - _orig.getY();
	if (BasicGeometry::equal(dx, 0.0))
		return INFINITY;
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