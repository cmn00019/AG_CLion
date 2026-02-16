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
	// d = |cross(AB, AP)| / |AB|
	double ABx = _dest.getX() - _orig.getX();
	double ABy = _dest.getY() - _orig.getY();
	double APx = v.getX() - _orig.getX();
	double APy = v.getY() - _orig.getY();

	double cross = ABx * APy - ABy * APx;
	double lengthAB = std::sqrt(ABx * ABx + ABy * ABy);

	if (lengthAB < glm::epsilon<double>())
		return 0.0;

	return std::abs(cross) / lengthAB;
}

bool Line::intersects(Line& line, Vect2d& intersection)
{
	// Cramer sin restricciones en t ni s (ambas son rectas infinitas)
	Point A = _orig;
	Point B = _dest;
	Point C = line.getA();
	Point D = line.getB();

	double v1x = B.getX() - A.getX();
	double v1y = B.getY() - A.getY();
	double v2x = D.getX() - C.getX();
	double v2y = D.getY() - C.getY();

	double det = v1x * v2y - v1y * v2x;

	if (std::abs(det) < glm::epsilon<double>())
		return false; // Rectas paralelas

	double t = ((C.getX() - A.getX()) * v2y - (C.getY() - A.getY()) * v2x) / det;

	intersection.setX(A.getX() + t * v1x);
	intersection.setY(A.getY() + t * v1y);

	return true;
}

bool Line::intersects(RayLine& rayline, Vect2d& intersection)
{
	// Cramer: sin restricción para la recta (t), con restricción s >= 0 para el rayo
	Point A = _orig;
	Point B = _dest;
	Point C = rayline.getA();
	Point D = rayline.getB();

	double v1x = B.getX() - A.getX();
	double v1y = B.getY() - A.getY();
	double v2x = D.getX() - C.getX();
	double v2y = D.getY() - C.getY();

	double det = v1x * v2y - v1y * v2x;

	if (std::abs(det) < glm::epsilon<double>())
		return false; // Paralelos

	double t = ((C.getX() - A.getX()) * v2y - (C.getY() - A.getY()) * v2x) / det;
	double s = ((C.getX() - A.getX()) * v1y - (C.getY() - A.getY()) * v1x) / det;

	// El rayo requiere s >= 0
	if (s < 0.0)
		return false;

	intersection.setX(A.getX() + t * v1x);
	intersection.setY(A.getY() + t * v1y);

	return true;
}

bool Line::intersects(SegmentLine& segment, Vect2d& intersection)
{
	// Cramer: sin restricción para la recta (t), con 0 <= s <= 1 para el segmento
	Point A = _orig;
	Point B = _dest;
	Point C = segment.getA();
	Point D = segment.getB();

	double v1x = B.getX() - A.getX();
	double v1y = B.getY() - A.getY();
	double v2x = D.getX() - C.getX();
	double v2y = D.getY() - C.getY();

	double det = v1x * v2y - v1y * v2x;

	if (std::abs(det) < glm::epsilon<double>())
		return false; // Paralelos

	double t = ((C.getX() - A.getX()) * v2y - (C.getY() - A.getY()) * v2x) / det;
	double s = ((C.getX() - A.getX()) * v1y - (C.getY() - A.getY()) * v1x) / det;

	// El segmento requiere 0 <= s <= 1
	if (s < 0.0 || s > 1.0)
		return false;

	intersection.setX(A.getX() + t * v1x);
	intersection.setY(A.getY() + t * v1y);

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