#include "stdafx.h"
#include "Edge3d.h"
#include "Ray3d.h"


Ray3d::Ray3d(Vect3d & orig, Vect3d & dest)
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
	os << "Ray --> Origin: " << ray._orig << "; destination: " << ray._dest;
	return os;
}

double Ray3d::distance(Vect3d& v){
    // book_dve: project v onto ray, clamp t >= 0
    Vect3d d = _dest.sub(_orig);    // direction
    Vect3d qp = v.sub(_orig);       // Q - P
    double t0 = d.dot(qp) / d.dot(d);

    if (t0 <= 0.0)
    {
        // Closest point is the ray origin
        return v.distance(_orig);
    }

    // Closest point is on the ray
    Vect3d proj = d.scalarMul(t0);
    Vect3d foot = _orig.add(proj);
    return v.distance(foot);
}

