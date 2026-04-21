#include "stdafx.h"
#include "Edge3d.h"
#include "Ray3d.h"


Ray3d::Ray3d(const Vect3d & orig, const Vect3d & dest)
	: Edge3d(orig, dest)
{
}

Ray3d::Ray3d(const Ray3d & ray)
	: Edge3d(ray)
{
}

Ray3d::~Ray3d()
{
}

Ray3d & Ray3d::operator=(const Ray3d & ray)
{
	if (this != &ray)
	{
		Edge3d::operator=(ray);
	}

	return *this;
}

std::ostream& operator<<(std::ostream& os, const Ray3d& ray)
{
	os << "Ray --> Origen: " << ray._orig << "; destino: " << ray._dest;
	return os;
}

double Ray3d::distance(const Vect3d& v) const {

    Vect3d d = _dest.sub(_orig);    // direccion
    Vect3d qp = v.sub(_orig);       // Q - P
    double t0 = d.dot(qp) / d.dot(d);

    if (t0 <= 0.0)
    {
        // Punto mas cercano es el origen del rayo
        return v.distance(_orig);
    }

    // el punto mas cercano se encuentra en el rayo
    Vect3d proj = d.scalarMul(t0);
    Vect3d foot = _orig.add(proj);
    return v.distance(foot);
}

