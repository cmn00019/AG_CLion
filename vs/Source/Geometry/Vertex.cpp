#include "stdafx.h"
#include "Polygon.h"

#include "Vertex.h"

// Public methods

Vertex::Vertex() : Point()
{
	_position = INVALID_POSITION;
	_polygon = nullptr;
}

Vertex::Vertex(const Point& point) : Point(point)
{
	_position = INVALID_POSITION;
	_polygon = nullptr;
}

Vertex::Vertex(const Point & point, Polygon* polygon, int pos) : Point(point)
{
	_position = pos;
	_polygon = polygon;
}

Vertex::~Vertex()
{
}

bool Vertex::convex()
{
	// XXXXX
	return false;
}

bool Vertex::concave()
{
	// XXXXX
	return false;
}

Vertex Vertex::next()
{
	// XXXXX
	return Vertex();
}

SegmentLine Vertex::nextEdge()
{
	// XXXXX
	return SegmentLine();
}

Vertex & Vertex::operator=(const Vertex & vertex)
{
	if (this != &vertex)
	{
		Point::operator=(vertex);
		this->_polygon = vertex._polygon;
		this->_position = vertex._position;
	}

	return *this;
}

std::ostream& operator<<(std::ostream& os, const Vertex& vertex)
{
	os << "Position: " << std::to_string(vertex._position);

	return os;
}

Vertex Vertex::previous()
{
	// XXXXX
	return Vertex();
}

SegmentLine Vertex::previousEdge()
{
	// XXXXX
	return SegmentLine();
}