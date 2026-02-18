#include "stdafx.h"
#include "Line.h"

#include "RayLine.h"
#include "SegmentLine.h"


Line::Line(const Point & a, const Point & b) : SegmentLine(a, b)
{
}

Line::Line(const Line & s) : SegmentLine(s)
{
}

Line::~Line()
{
}

double Line::distancePointLine(Vect2d& v)
{
	// P = _orig, Q = _dest
	// d = Q - P (vector director de la recta)
	Vect2d d(_dest.getX() - _orig.getX(), _dest.getY() - _orig.getY());

	// A - P
	Vect2d ap(v.getX() - _orig.getX(), v.getY() - _orig.getY());

	// t0 = d·(A-P) / (d·d)
	double dd = d.dot(d);

	if (BasicGeometry::equal(dd, 0.0))
		return 0.0;

	double t0 = d.dot(ap) / dd;

	// D = |A - (P + t0*d)| (para la recta, siempre se usa la proyeccion)
	Vect2d proj = Vect2d(_orig) + d.scalarMult(t0);
	return v.distance(proj);
}

bool Line::intersects(Line& line, Vect2d& intersection)
{
	Vect2d c(line.getA());
	Vect2d d(line.getB());
	double s, t;

	// Recta-Recta: cualquier valor de s y t es valido
	if (intersects(c, d, s, t))
	{
		Point p = getPoint(s);
		intersection.setX(p.getX());
		intersection.setY(p.getY());
		return true;
	}
	return false;
}

bool Line::intersects(RayLine& rayline, Vect2d& intersection)
{
	Vect2d c(rayline.getA());
	Vect2d d(rayline.getB());
	double s, t;

	// Recta-Rayo: t >= 0
	if (intersects(c, d, s, t) && t >= 0.0)
	{
		Point p = getPoint(s);
		intersection.setX(p.getX());
		intersection.setY(p.getY());
		return true;
	}
	return false;
}

bool Line::intersects(SegmentLine& segment, Vect2d& intersection)
{
	Vect2d c(segment.getA());
	Vect2d d(segment.getB());
	double s, t;

	// Recta-Segmento: 0 <= t <= 1
	if (intersects(c, d, s, t) && t >= 0.0 && t <= 1.0)
	{
		Point p = getPoint(s);
		intersection.setX(p.getX());
		intersection.setY(p.getY());
		return true;
	}
	return false;
}

// Protected method

bool Line::intersects(Vect2d& c, Vect2d& d, double& s, double& t)
{
	double xAB = _dest.getX() - _orig.getX();
	double yAB = _dest.getY() - _orig.getY();
	double xCD = d.getX() - c.getX();
	double yCD = d.getY() - c.getY();
	double xAC = c.getX() - _orig.getX();
	double yAC = c.getY() - _orig.getY();

	double denom = xCD * yAB - xAB * yCD;

	if (BasicGeometry::equal(xCD * yAB, xAB * yCD))
		return false; // Rectas paralelas

	s = (xCD * yAC - xAC * yCD) / denom;
	t = (xAB * yAC - xAC * yAB) / denom;

	// Para una recta, s puede tomar cualquier valor (no hay restriccion)
	return true;
}

bool Line::incorrectSegmentIntersection(SegmentLine& l)
{
	return false;
}

Line & Line::operator=(const Line & line)
{
	if (this != &line)
	{
		SegmentLine::operator=(line);
	}

	return *this;
}

std::ostream& operator<<(std::ostream& os, const Line& line)
{
	os << "Line --->\n\tPoint A: " << line._orig << "\n\tPoint B: " << line._dest << "\n";

	return os;
}

bool Line::segmentIntersection(SegmentLine& l)
{
	// Override: para una recta, solo comprobamos que C y D están en lados opuestos
	// No necesitamos la segunda condición (A,B respecto a CD) porque la recta es infinita
	Point c = l.getA();
	Point d = l.getB();

	if (c.colinear(_orig, _dest) || d.colinear(_orig, _dest))
		return false;

	return (c.left(_orig, _dest) ^ d.left(_orig, _dest));
}