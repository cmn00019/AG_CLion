#include "stdafx.h"
#include "Polygon.h"

// Public methods

Polygon::Polygon()
{
}

Polygon::Polygon(const Polygon& Polygon)
{
	_vertices = std::vector<Vertex>(Polygon._vertices);
}

Polygon::Polygon(std::vector<Vertex>& vertices)
{
	_vertices = std::vector<Vertex>(vertices);
}

SegmentLine Polygon::getEdge(int i)
{
	return SegmentLine(getVertexAt(i), getVertexAt((i + 1) % _vertices.size()));
}

Polygon::Polygon(const std::string & filename)
{
	//XXXX
}

Polygon::~Polygon()
{
}

bool Polygon::add(const Vertex & vertex)
{
	size_t index = _vertices.size();

	//if (intersectsWithAnySegment(vertex)) return false;

	_vertices.push_back(vertex);
	_vertices[index].setPolygon(this);
	_vertices[index].setPosition(static_cast<int>(index));

	return true;
}

bool Polygon::add(const Point & point)
{
	Vertex vertex(point);

	return this->add(vertex);
}

Vertex Polygon::getVertexAt(int position)
{
	if (position >= 0 && position < _vertices.size())
	{
		return _vertices[position];
	}
	else 
	{
		return Vertex();
	}
}

bool Polygon::intersects(Line& line, Vect2d& interseccion)
{
	// XXXX
	return false;
}

bool Polygon::intersects(RayLine& ray, Vect2d& interseccion)
{
	// XXXX
	return false;
}

bool Polygon::convex()
{
	// XXXX
	return true;
}

bool Polygon::intersects(SegmentLine& segment, Vect2d& interseccion)
{
	return false;
}

Vertex Polygon::next(int index)
{
	if (index >= 0 && index < _vertices.size())
	{
		return _vertices[(index + 1) % _vertices.size()];
	}

	return Vertex();
}

std::ostream& operator<<(std::ostream& os, const Polygon& polygon)
{
	for (int i = 0; i < polygon._vertices.size(); i++) 
	{
		os << polygon._vertices[i] << "\n";
	}

	return os;
}

Vertex Polygon::previous(int index)
{
	if (index >= 0 && index < _vertices.size())
	{
		return _vertices[(index - 1 + _vertices.size()) % _vertices.size()];
	}

	return Vertex();
}

Polygon & Polygon::operator=(const Polygon &polygon)
{
	if (this != &polygon)
	{
		this->_vertices = polygon._vertices;
	}

	return *this;
}

bool Polygon::pointInConvexPolygonGeo(Point& point)
{
    // XXXX
	return true;
}

void Polygon::save(const std::string& filename)
{
	// XXXX
}

void Polygon::set(Vertex& vertex, int pos)
{
	if (pos >= 0 && pos < _vertices.size()) {
		_vertices[pos] = vertex;
		vertex.setPolygon(this);
		vertex.setPosition(pos);
	}
}
