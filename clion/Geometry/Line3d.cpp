#include "stdafx.h"
#include "BasicGeometry.h"
#include "Line3d.h"

Line3d::Line3d()
{
}

Line3d::Line3d(const Vect3d & orig, const Vect3d & dest)
	: Edge3d(orig, dest)
{
}

Line3d::Line3d(const Line3d & line)
	: Edge3d(line)
{
}

Line3d::~Line3d()
{
}


double Line3d::distance(const Line3d & line) const
{
    // d = |(q-q')·(v×v')| / ||v×v'||
    Vect3d v = _dest.sub(_orig);          // direccion de la linea
    Vect3d vp = line._dest.sub(line._orig);
    Vect3d cross = v.xProduct(vp);        // v × v'
    double crossMod = cross.module();

    if (BasicGeometry::equal(crossMod, 0.0))
    {
        // Lineas paralelas
        Vect3d diff = line._orig.sub(_orig); // q - q'
        Vect3d projected = v.scalarMul(diff.dot(v) / v.dot(v));
        Vect3d perp = diff.sub(projected);
        return perp.module();
    }

    Vect3d diff = _orig.sub(line._orig);  // q - q'
    double num = std::abs(diff.dot(cross));
    return num / crossMod;
}

Line3d Line3d::normalLine(const Vect3d & point) const
{
    //  λ = v·(p-t)/(v·v), Q = t + λv
    Vect3d v = _dest.sub(_orig);
    Vect3d pt = point.sub(_orig);
    double lambda = v.dot(pt) / v.dot(v);
    Vect3d lambdaV = v.scalarMul(lambda);
    Vect3d foot = _orig.add(lambdaV);
    return Line3d(foot, point);
}

double Line3d::distance(const Vect3d& p) const {
    // λ = v·(p-t)/(v·v), dist = ||p - t - λv||
    Vect3d v = _dest.sub(_orig);
    Vect3d pt = p.sub(_orig);
    double lambda = v.dot(pt) / v.dot(v);
    Vect3d lambdaV = v.scalarMul(lambda);
    Vect3d foot = _orig.add(lambdaV);
    Vect3d diff = p.sub(foot);
    return diff.module();
}

Line3d & Line3d::operator=(const Line3d & line)
{
	if (this != &line)
	{
		Edge3d::operator=(line);
	}

	return *this;
}

std::ostream& operator<<(std::ostream& os, const Line3d& line)
{
	std::cout << "Linea -> Origen: " << line._orig << ", Destino: " << line._dest;
	return os;
}

