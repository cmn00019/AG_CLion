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
	if (!_polygon) return false;

	Vertex prev = _polygon->previous(_position);
	Vertex nxt  = _polygon->next(_position);

	Point prevP = prev.getPoint();
	Point thisP = this->getPoint();
	Point nextP = nxt.getPoint();

	// Misma fórmula que Polygon::convex(): curr.triangleArea2(next, prev)
	// Para CCW, cross > 0 indica vértice convexo
	double cross = thisP.triangleArea2(nextP, prevP);
	return cross > 0;
}

bool Vertex::concave()
{
	return !convex();
}

Vertex Vertex::next()
{
	if (_polygon)
	{
		return _polygon->next(_position);
	}
	return Vertex();
}

SegmentLine Vertex::nextEdge()
{
	if (_polygon)
	{
		// Arista desde this hasta next = getEdge(_position)
		return _polygon->getEdge(_position);
	}
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
	if (_polygon)
	{
		return _polygon->previous(_position);
	}
	return Vertex();
}

SegmentLine Vertex::previousEdge()
{
	if (_polygon)
	{
		// Arista desde previous hasta this = getEdge(prevIndex)
		int prevIdx = (_position - 1 + static_cast<int>(_polygon->getNumVertices())) % static_cast<int>(_polygon->getNumVertices());
		return _polygon->getEdge(prevIdx);
	}
	return SegmentLine();
}