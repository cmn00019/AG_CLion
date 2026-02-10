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
	// XXXX
	return false;
}

double SegmentLine::getC()
{
	// XXXX
	return 0.0;
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
    // XXXX
	return Point();
}

bool SegmentLine::impSegmentIntersection(SegmentLine& segment)
{
	// XXXX
    return true;
}

bool SegmentLine::isHorizontal()
{
	// XXXX
	return true;
}

bool SegmentLine::isVertical()
{
    // XXXX
	return true; 
}

double SegmentLine::slope()
{
	// XXXX   
    return 0;
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