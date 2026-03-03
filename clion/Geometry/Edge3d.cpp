#include "stdafx.h"
#include "Edge3d.h"


Edge3d::Edge3d()
{
}

Edge3d::Edge3d(Vect3d & orig, Vect3d & dest)
	: _orig(orig), _dest(dest)
{
}

Edge3d::Edge3d(const Edge3d & edge)
{
	_orig = edge._orig;
	_dest = edge._dest;
}

Edge3d::~Edge3d()
{
}

Vect3d Edge3d::getDestination()
{
	return _dest;
}

Vect3d Edge3d::getOrigin()
{
	return _orig;
}

Vect3d Edge3d::getPoint(double t)
{
	if (!isTvalid(t)) return Vect3d();
        Vect3d aux1 = _dest.sub(_orig);
        Vect3d aux2 = aux1.scalarMul(t);

	return Vect3d(_orig.add(aux2));
}

std::vector<double> Edge3d::getVertices()
{
	std::vector<double> vertices;
	std::vector<double> origVertices = _orig.getVert(), destVertices = _dest.getVert();
	vertices.insert(vertices.end(), origVertices.begin(), origVertices.end());
	vertices.insert(vertices.end(), destVertices.begin(), destVertices.end());

	return vertices;
}

Edge3d & Edge3d::operator=(const Edge3d & edge)
{
	_orig = edge._orig;
	_dest = edge._dest;

	return *this;
}

std::ostream& operator<<(std::ostream& os, const Edge3d& edge)
{
	os << "Edge --> Origin: " << edge._orig << "; destination: " << edge._dest;
	return os;
}
