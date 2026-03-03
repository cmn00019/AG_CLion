#include "stdafx.h"
#include "Segment3d.h"


Segment3d::Segment3d(Vect3d & orig, Vect3d & dest)
	: Edge3d(orig, dest)
{
}

Segment3d::Segment3d(const Segment3d & segment)
	: Edge3d(segment)
{
}

Segment3d::~Segment3d()
{
}

Segment3d & Segment3d::operator=(const Segment3d & segment)
{
	if (this != &segment)
	{
		Edge3d::operator=(segment);
	}

	return *this;
}

std::ostream& operator<<(std::ostream& os, const Segment3d& segment)
{
	std::cout << "Segment -> Origin: " << segment._orig << ", " << ", Destination: " << segment._dest;
	return os;
}

double Segment3d::distance(Vect3d& v){
    // book_dve: project v onto segment, clamp t to [0, 1]
    Vect3d d = _dest.sub(_orig);    // direction P0 -> P1
    Vect3d qp = v.sub(_orig);       // Q - P0
    double t0 = d.dot(qp) / d.dot(d);

    if (t0 <= 0.0)
    {
        return v.distance(_orig);
    }
    if (t0 >= 1.0)
    {
        return v.distance(_dest);
    }

    Vect3d proj = d.scalarMul(t0);
    Vect3d foot = _orig.add(proj);
    return v.distance(foot);
}

