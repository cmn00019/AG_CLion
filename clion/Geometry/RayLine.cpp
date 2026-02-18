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

double RayLine::distPointRay(Vect2d& vector)
{
	// P = _orig, Q = _dest
	// d = Q - P (vector director del rayo)
	Vect2d d(_dest.getX() - _orig.getX(), _dest.getY() - _orig.getY());

	// A - P
	Vect2d ap(vector.getX() - _orig.getX(), vector.getY() - _orig.getY());

	// t0 = d·(A-P) / (d·d)
	double dd = d.dot(d);

	if (BasicGeometry::equal(dd, 0.0))
		return vector.distance(_orig);

	double t0 = d.dot(ap) / dd;

	if (t0 <= 0.0)
	{
		// Punto mas cercano es P (_orig)
		return vector.distance(_orig);
	}
	else
	{
		// Punto mas cercano es P + t0 * d
		Vect2d proj = Vect2d(_orig) + d.scalarMult(t0);
		return vector.distance(proj);
	}
}

bool RayLine::incorrectSegmentIntersection(SegmentLine& segment)
{
	return false;
}

bool RayLine::intersects(Line& line, Vect2d& intersection)
{
	Vect2d c(line.getA());
	Vect2d d(line.getB());
	double s, t;

	// Rayo-Recta: cualquier valor de t es valido
	if (intersects(c, d, s, t))
	{
		Point p = getPoint(s);
		intersection.setX(p.getX());
		intersection.setY(p.getY());
		return true;
	}
	return false;
}

bool RayLine::intersects(RayLine& rayline, Vect2d& intersection)
{
	Vect2d c(rayline.getA());
	Vect2d d(rayline.getB());
	double s, t;

	// Rayo-Rayo: t >= 0
	if (intersects(c, d, s, t) && t >= 0.0)
	{
		Point p = getPoint(s);
		intersection.setX(p.getX());
		intersection.setY(p.getY());
		return true;
	}
	return false;
}

bool RayLine::intersects(SegmentLine& segment, Vect2d& intersection)
{
	Vect2d c(segment.getA());
	Vect2d d(segment.getB());
	double s, t;

	// Rayo-Segmento: 0 <= t <= 1
	if (intersects(c, d, s, t) && t >= 0.0 && t <= 1.0)
	{
		Point p = getPoint(s);
		intersection.setX(p.getX());
		intersection.setY(p.getY());
		return true;
	}
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

// Protected method

bool RayLine::intersects(Vect2d& c, Vect2d& d, double& s, double& t)
{
	double xAB = _dest.getX() - _orig.getX();
	double yAB = _dest.getY() - _orig.getY();
	double xCD = d.getX() - c.getX();
	double yCD = d.getY() - c.getY();
	double xAC = c.getX() - _orig.getX();
	double yAC = c.getY() - _orig.getY();

	double denom = xCD * yAB - xAB * yCD;

	if (std::abs(denom) < glm::epsilon<double>())
		return false; // Paralelos

	s = (xCD * yAC - xAC * yCD) / denom;
	t = (xAB * yAC - xAC * yAB) / denom;

	// Para un rayo, s debe ser >= 0
	if (s < 0.0)
		return false;

	return true;
}
