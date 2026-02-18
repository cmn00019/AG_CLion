#include "stdafx.h"
#include "Polygon.h"
#include <fstream>

// --- MÉTODOS PÚBLICOS ---

Polygon::Polygon()
{
}

Polygon::Polygon(const Polygon& Polygon)
{
    _vertices = std::vector<Vertex>(Polygon._vertices);

    // Actualizar el doble enlace: los vértices copiados deben apuntar a this
    for (size_t i = 0; i < _vertices.size(); i++)
    {
        _vertices[i].setPolygon(this);
        _vertices[i].setPosition(static_cast<int>(i));
    }
}

Polygon::Polygon(std::vector<Vertex>& vertices)
{
    _vertices = std::vector<Vertex>(vertices);

    // Actualizar el doble enlace: los vértices deben apuntar a this
    for (size_t i = 0; i < _vertices.size(); i++)
    {
        _vertices[i].setPolygon(this);
        _vertices[i].setPosition(static_cast<int>(i));
    }
}

Polygon::Polygon(const std::string & filename)
{
    std::ifstream file(filename);
    if (file.is_open())
    {
        double x, y;
        while (file >> x >> y)
        {
            add(Point(x, y));
        }
        file.close();
    }
}

Polygon::~Polygon()
{
}

SegmentLine Polygon::getEdge(int i)
{
    return SegmentLine(getVertexAt(i), getVertexAt((i + 1) % _vertices.size()));
}

bool Polygon::add(const Vertex & vertex)
{
    size_t index = _vertices.size();
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

    return Vertex();
}

// --- MÉTODOS DE INTERSECCIÓN ---

bool Polygon::intersects(Line& line, Vect2d& interseccion)
{
    for (int i = 0; i < _vertices.size(); i++)
    {
        SegmentLine edge = getEdge(i);

        if (line.intersects(edge, interseccion)) {
            return true;
        }
    }
    return false;
}

bool Polygon::intersects(RayLine& ray, Vect2d& interseccion)
{
    for (int i = 0; i < _vertices.size(); i++)
    {
        SegmentLine edge = getEdge(i);

        if (edge.intersects(ray, interseccion)) {
            return true;
        }
    }
    return false;
}

bool Polygon::intersects(SegmentLine& segment, Vect2d& interseccion)
{
    for (int i = 0; i < _vertices.size(); i++)
    {
        SegmentLine edge = getEdge(i);

        if (edge.intersects(segment, interseccion)) {
            return true;
        }
    }
    return false;
}

// --- CONVEXIDAD Y PUNTO DENTRO ---

bool Polygon::convex()
{
    if (_vertices.size() < 3) return false;

    bool hasPositive = false;
    bool hasNegative = false;

    for (int i = 0; i < _vertices.size(); i++)
    {
        Vertex prev = getVertexAt((i - 1 + _vertices.size()) % _vertices.size());
        Vertex curr = getVertexAt(i);
        Vertex next = getVertexAt((i + 1) % _vertices.size());

        double crossProduct = curr.triangleArea2(next, prev);

        if (crossProduct > 0) hasPositive = true;
        if (crossProduct < 0) hasNegative = true;

        // Si hay giros a izquierda y a derecha mezclados, es cóncavo.
        if (hasPositive && hasNegative) return false;
    }

    return true;
}

bool Polygon::pointInConvexPolygonGeo(Point& point)
{
    if (!convex()) return false;
    if (_vertices.empty()) return false;

    SegmentLine edge0 = getEdge(0);
    bool sideReference = point.left(edge0.getA(), edge0.getB());

    for (int i = 1; i < _vertices.size(); i++)
    {
        SegmentLine edge = getEdge(i);
        if (point.left(edge.getA(), edge.getB()) != sideReference)
        {
            return false;
        }
    }

    return true;
}

// --- UTILIDADES ---

Vertex Polygon::next(int index)
{
    if (index >= 0 && index < _vertices.size())
    {
       return _vertices[(index + 1) % _vertices.size()];
    }
    return Vertex();
}

Vertex Polygon::previous(int index)
{
    if (index >= 0 && index < _vertices.size())
    {
       return _vertices[(index - 1 + _vertices.size()) % _vertices.size()];
    }
    return Vertex();
}

void Polygon::save(const std::string& filename)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        for (Vertex& v : _vertices)
        {
            file << v.getX() << " " << v.getY() << "\n";
        }
        file.close();
    }
}

void Polygon::set(Vertex& vertex, int pos)
{
    if (pos >= 0 && pos < _vertices.size()) {
       _vertices[pos] = vertex;
       _vertices[pos].setPolygon(this);
       _vertices[pos].setPosition(pos);
    }
}

Polygon & Polygon::operator=(const Polygon &polygon)
{
    if (this != &polygon)
    {
       this->_vertices = polygon._vertices;

       // Actualizar el doble enlace tras la copia
       for (size_t i = 0; i < _vertices.size(); i++)
       {
           _vertices[i].setPolygon(this);
           _vertices[i].setPosition(static_cast<int>(i));
       }
    }

    return *this;
}

std::ostream& operator<<(std::ostream& os, const Polygon& polygon)
{
    for (const auto& v : polygon._vertices)
    {
       os << v << "\n";
    }
    return os;
}