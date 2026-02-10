#include "stdafx.h"
#include "RayLine.h"

#include "Line.h"

// Public methods

RayLine::RayLine(const Point& a, const Point& b) : SegmentLine(a, b)
{
}

RayLine::RayLine(const RayLine& ray) : SegmentLine(ray)
{
}

RayLine::~RayLine()
{
}

float RayLine::distanceToPoint(Vect2d& vector)
{
	return 0.0f;
}

bool RayLine::incorrectSegmentIntersection(SegmentLine& segment)
{
	return false;
}

RayLine & RayLine::operator=(const RayLine & rayline)
{
	if (this != &rayline)
	{
		SegmentLine::operator=(rayline);
	}

	return *this;
}

std::ostream& operator<<(std::ostream& os, const RayLine& ray)
{
	os << "Point A: " << ray._orig << ", Point B: " << ray._dest << "\n";

	return os;
}