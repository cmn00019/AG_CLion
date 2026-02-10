#include "stdafx.h"
#include "Line.h"

#include "RayLine.h"
#include "SegmentLine.h"


Line::Line(const Point & a, const Point & b) : SegmentLine(a, b)
{
}

Line::Line(const Line & s) : SegmentLine(s)
{
}

Line::~Line()
{
}

double Line::distancePointLine(Vect2d& v)
{
	// XXXX
	return 0.0;
}

bool Line::intersects(Line& line, Vect2d& intersection)
{
	// XXXX
	return false;
}

bool Line::intersects(RayLine& rayline, Vect2d& intersection)
{
	// XXXX
	return false;
}

bool Line::intersects(SegmentLine& segment, Vect2d& intersection)
{
	// XXXX
	return false;
}

bool Line::incorrectSegmentIntersection(SegmentLine& l)
{
	return false;
}

Line & Line::operator=(const Line & line)
{
	if (this != &line)
	{
		SegmentLine::operator=(line);
	}

	return *this;
}

std::ostream& operator<<(std::ostream& os, const Line& line)
{
	os << "Line --->\n\tPoint A: " << line._orig << "\n\tPoint B: " << line._dest << "\n";

	return os;
}

bool Line::segmentIntersection(SegmentLine& l)
{
	// XXXX
	return false;
}