#include "stdafx.h"
#include "SegmentLine.h"

#include "Line.h"
#include "RayLine.h"

// Public methods

SegmentLine::SegmentLine()
{
}

SegmentLine::SegmentLine(const Point& a, const Point& b) : _orig(a), _dest(b)
{
}

SegmentLine::SegmentLine(const SegmentLine& segment) : _orig(segment._orig), _dest(segment._dest)
{
}

SegmentLine::SegmentLine(double ax, double ay, double bx, double by) : _orig(ax, ay), _dest(bx, by)
{
}

SegmentLine::~SegmentLine()
{
}

SegmentLine & SegmentLine::operator=(const SegmentLine & segment)
{
	if (this != &segment)
	{
		this->_orig = segment._orig;
		this->_dest = segment._dest;
	}

	return *this;
}

bool SegmentLine::segmentIntersection(SegmentLine& l)
{
	Point a = this->_orig;
	Point b = this->_dest;
	Point c = l._orig;
	Point d = l._dest;

	if (a.colinear(c,d) || b.colinear(c,d) ||
		c.colinear(a,b) || d.colinear(a,b))
		return false;
	return (a.left(c,d) ^ b.left(c,d) &&
		c.left(a,b) ^ d.left(a,b));
}

double SegmentLine::getC()
{
	if (isVertical())
		return std::numeric_limits<double>::infinity();

	double m = slope();

	return _orig.getY() - m * _orig.getX();
}

bool SegmentLine::distinct(SegmentLine & segment)
{
	return !equal(segment);
}

double SegmentLine::distPointSegment(Vect2d& vector)
{
	// P = _orig, Q = _dest
	// d = Q - P (vector director del segmento)
	Vect2d d(_dest.getX() - _orig.getX(), _dest.getY() - _orig.getY());

	// A - P (vector del origen del segmento al punto)
	Vect2d ap(vector.getX() - _orig.getX(), vector.getY() - _orig.getY());

	// t0 = d·(A-P) / (d·d)
	double dd = d.dot(d);

	if (BasicGeometry::equal(dd, 0.0))
	{
		// (longitud 0)
		return vector.distance(_orig);
	}

	double t0 = d.dot(ap) / dd;

	if (t0 <= 0.0)
	{
		// Punto mas cercano es P (_orig)
		return vector.distance(_orig);
	}
	else if (t0 >= 1.0)
	{
		// Punto mas cercano es Q (_dest)
		return vector.distance(_dest);
	}
	else
	{
		// Punto mas cercano es P + t0 * d
		Vect2d proj = Vect2d(_orig) + d.scalarMult(t0);
		return vector.distance(proj);
	}
}

bool SegmentLine::equal(SegmentLine & segment)
{
	return (_orig.equal(segment._orig) && _dest.equal(segment._dest)) || (_orig.equal(segment._dest) && _dest.equal(segment._orig));
}

Point SegmentLine::getPoint(double t)
{
    double x = _orig.getX() + t * (_dest.getX() - _orig.getX());
	double y = _orig.getY() + t * (_dest.getY() - _orig.getY());

	return Point(x, y);
}

bool SegmentLine::impSegmentIntersection(SegmentLine& segment)
{
	Point a = this->_orig;
	Point b = this->_dest;
	Point c = segment._orig;
	Point d = segment._dest;

	Point::PointClassification cl;

	// 1. ¿Está C contenido en AB?
	cl = c.classify(a, b);
	if (cl == Point::PointClassification::BETWEEN ||
		cl == Point::PointClassification::ORIGIN ||
		cl == Point::PointClassification::DEST) return true;

	// 2. ¿Está D contenido en AB?
	cl = d.classify(a, b);
	if (cl == Point::PointClassification::BETWEEN ||
		cl == Point::PointClassification::ORIGIN ||
		cl == Point::PointClassification::DEST) return true;

	// 3. ¿Está A contenido en CD?
	cl = a.classify(c, d);
	if (cl == Point::PointClassification::BETWEEN ||
		cl == Point::PointClassification::ORIGIN ||
		cl == Point::PointClassification::DEST) return true;

	// 4. ¿Está B contenido en CD?
	cl = b.classify(c, d);
	if (cl == Point::PointClassification::BETWEEN ||
		cl == Point::PointClassification::ORIGIN ||
		cl == Point::PointClassification::DEST) return true;

	return false;
}

bool SegmentLine::isHorizontal()
{
	return BasicGeometry::equal(this->_orig.getY(), this->_dest.getY());
}

bool SegmentLine::isVertical()
{
	return BasicGeometry::equal(this->_orig.getX(), this->_dest.getX());
}

double SegmentLine::slope()
{
	if (isVertical())
		return std::numeric_limits<double>::infinity();

	double dx = _dest.getX() - _orig.getX();
	double dy = _dest.getY() - _orig.getY();

	return dy / dx;
}

// Protected methods

std::ostream& operator<<(std::ostream& os, const SegmentLine& segment)
{
	os << "Point A: " << segment._orig << ", Point B: " << segment._dest << "\n";

	return os;
}

// Protected methods

float SegmentLine::getDistanceT0(Vect2d& point)
{
	// d = Q - P (vector director del segmento)
	Vect2d d(_dest.getX() - _orig.getX(), _dest.getY() - _orig.getY());

	// A - P
	Vect2d ap(point.getX() - _orig.getX(), point.getY() - _orig.getY());

	// t0 = d·(A-P) / (d·d)
	double dd = d.dot(d);

	if (BasicGeometry::equal(dd, 0.0))
		return 0.0f;

	return static_cast<float>(d.dot(ap) / dd);
}

bool SegmentLine::intersects(Vect2d& c, Vect2d& d, double& s, double& t)
{
	// A = _orig, B = _dest (segmento this)
	// C = c, D = d (la otra linea/segmento/rayo)

	double xAB = _dest.getX() - _orig.getX();
	double yAB = _dest.getY() - _orig.getY();
	double xCD = d.getX() - c.getX();
	double yCD = d.getY() - c.getY();
	double xAC = c.getX() - _orig.getX();
	double yAC = c.getY() - _orig.getY();

	// Denominador comun
	double denom = xCD * yAB - xAB * yCD;

	// Si el denominador es ~0, las rectas son paralelas
	if (BasicGeometry::equal(xCD * yAB, xAB * yCD))
		return false;

	// Calcular s (parametro de this: AB)
	s = (xCD * yAC - xAC * yCD) / denom;

	// Calcular t (parametro de la otra: CD)
	t = (xAB * yAC - xAC * yAB) / denom;

	// s debe estar en [0,1] para que la interseccion caiga en el segmento this
	if (s < 0.0 || s > 1.0)
		return false;

	return true;
}

// --- Metodos publicos de interseccion ---

bool SegmentLine::intersects(Line& r, Vect2d& res)
{
	Vect2d c(r.getA());
	Vect2d d(r.getB());
	double s, t;

	// Segmento-Recta: solo necesitamos 0 <= s <= 1 (t sin restriccion)
	if (intersects(c, d, s, t))
	{
		Point p = getPoint(s);
		res.setX(p.getX());
		res.setY(p.getY());
		return true;
	}
	return false;
}

bool SegmentLine::intersects(RayLine& r, Vect2d& res)
{
	Vect2d c(r.getA());
	Vect2d d(r.getB());
	double s, t;

	// Segmento-Rayo: 0 <= s <= 1 y t >= 0
	if (intersects(c, d, s, t) && t >= 0.0)
	{
		Point p = getPoint(s);
		res.setX(p.getX());
		res.setY(p.getY());
		return true;
	}
	return false;
}

bool SegmentLine::intersects(SegmentLine& r, Vect2d& res)
{
	Vect2d c(r.getA());
	Vect2d d(r.getB());
	double s, t;

	// Segmento-Segmento: 0 <= s <= 1 y 0 <= t <= 1
	if (intersects(c, d, s, t) && t >= 0.0 && t <= 1.0)
	{
		Point p = getPoint(s);
		res.setX(p.getX());
		res.setY(p.getY());
		return true;
	}
	return false;
}
