#include "stdafx.h"
#include "Circle.h"

// Public methods

Circle::~Circle()
{
}

bool Circle::isInside(const Point &p)
{ 
    return BasicGeometry::equal(_radius, _center.distance(const_cast<Point&>(p)));
}

Polygon Circle::getPointsCircle(uint32_t numSubdivisions)
{        
    Polygon polygon;
    float angle = .0f, alpha = 2.0f * glm::pi<float>() / numSubdivisions;

    while (angle < (2.0f * glm::pi<float>() + glm::epsilon<float>()))
    {
        Point point = Point(_center.getX() + _radius * std::cos(angle),
                            _center.getY() - _radius * std::sin(angle));
        polygon.add(point);
        angle += alpha;
    }

    return polygon;
}

Circle& Circle::operator=(const Circle& circle)
{
    if (this != &circle)
    {
        this->_center = circle._center;
        this->_radius = circle._radius;
    }

    return *this;
}

// --- Relacion entre dos circulos ---

RelationCircles Circle::relacionaCir(Circle& c)
{
    double dist = _center.distance(c._center);
    double r1 = _radius;
    double r2 = c._radius;

    // CONCENTRICO: centros iguales
    if (BasicGeometry::equal(dist, 0.0))
        return CONCENTRIC;

    // EXTERIOR: la distancia es mayor que la suma de radios
    if (dist > r1 + r2)
        return EXTERNAL;

    // TANGENTE EXTERIOR: la distancia es igual a la suma de radios
    if (BasicGeometry::equal(dist, r1 + r2))
        return EXTERNAL_TANG;

    // TANGENTE INTERIOR: la distancia es igual a |r2 - r1|
    if (BasicGeometry::equal(dist, std::abs(r2 - r1)))
        return INTERIOR_TANG;

    // INTERIOR: la distancia es menor que |r2 - r1|
    if (dist < std::abs(r2 - r1))
        return INTERNAL;

    // SECANTE: r2 - r1 < dist < r1 + r2
    return SECANT;
}

// --- Relacion entre circulo y recta ---

RelationCircleLine Circle::relacionaLine(Line& l)
{
    Vect2d center(_center);
    double dist = l.distancePointLine(center);

    if (dist < _radius && !BasicGeometry::equal(dist, _radius))
        return INTERSECT;

    if (BasicGeometry::equal(dist, _radius))
        return TANGENTS;

    return NO_INTERSECT;
}

// --- Interseccion circulo-recta (sin restriccion en t) ---

RelationCircleLine Circle::intersect(Line& l, Vect2d& pinter1, Vect2d& pinter2)
{
    // L = P + t * d, con P = l.getA(), d = l.getB() - l.getA()
    // |X - C|^2 = r^2
    // Delta = P - C
    // delta = (d . Delta)^2 - |d|^2 * (|Delta|^2 - r^2)
    // t = (-d . Delta +- sqrt(delta)) / |d|^2

    // d = B - A (vector director)
    Vect2d d(l.getB().getX() - l.getA().getX(), l.getB().getY() - l.getA().getY());

    // Delta = P - C
    Vect2d delta(l.getA().getX() - _center.getX(), l.getA().getY() - _center.getY());

    // d . Delta
    double dDotDelta = d.dot(delta);

    // |d|^2
    double dSq = d.dot(d);

    // |Delta|^2
    double deltaSq = delta.dot(delta);

    // discriminante: delta = (d . Delta)^2 - |d|^2 * (|Delta|^2 - r^2)
    double disc = dDotDelta * dDotDelta - dSq * (deltaSq - _radius * _radius);


    if (BasicGeometry::equal(disc, 0.0))
    {
        // Tangente: un punto
        double t = -dDotDelta / dSq;
        Vect2d p = Vect2d(l.getA()) + d.scalarMult(t);
        pinter1 = p;
        pinter2 = p;
        return TANGENTS;
    }

    if (disc < 0.0)
        return NO_INTERSECT;


    // Secante: dos puntos
    double sqrtDisc = std::sqrt(disc);
    double t1 = (-dDotDelta - sqrtDisc) / dSq;
    double t2 = (-dDotDelta + sqrtDisc) / dSq;

    pinter1 = Vect2d(l.getA()) + d.scalarMult(t1);
    pinter2 = Vect2d(l.getA()) + d.scalarMult(t2);

    return INTERSECT;
}

// --- Interseccion circulo-segmento (0 <= t <= 1) ---

RelationCircleLine Circle::intersect(SegmentLine& s, Vect2d& pinter1, Vect2d& pinter2)
{
    Vect2d d(s.getB().getX() - s.getA().getX(), s.getB().getY() - s.getA().getY());
    Vect2d delta(s.getA().getX() - _center.getX(), s.getA().getY() - _center.getY());

    double dDotDelta = d.dot(delta);
    double dSq = d.dot(d);
    double deltaSq = delta.dot(delta);

    double disc = dDotDelta * dDotDelta - dSq * (deltaSq - _radius * _radius);


    if (BasicGeometry::equal(disc, 0.0))
    {
        double t = -dDotDelta / dSq;
        if (t >= 0.0 && t <= 1.0)
        {
            pinter1 = Vect2d(s.getA()) + d.scalarMult(t);
            pinter2 = pinter1;
            return TANGENTS;
        }
        return NO_INTERSECT;
    }

    if (disc < 0.0)
        return NO_INTERSECT;

    int count = 0;

    double sqrtDisc = std::sqrt(disc);
    double t1 = (-dDotDelta - sqrtDisc) / dSq;
    double t2 = (-dDotDelta + sqrtDisc) / dSq;

    if (t1 >= 0.0 && t1 <= 1.0)
    {
        pinter1 = Vect2d(s.getA()) + d.scalarMult(t1);
        count++;
    }

    if (t2 >= 0.0 && t2 <= 1.0)
    {
        if (count == 0)
        {
            pinter1 = Vect2d(s.getA()) + d.scalarMult(t2);
        }
        else
        {
            pinter2 = Vect2d(s.getA()) + d.scalarMult(t2);
        }
        count++;
    }

    if (count == 0) return NO_INTERSECT;
    if (count == 1) { pinter2 = pinter1; return TANGENTS; }
    return INTERSECT;
}

// --- Interseccion circulo-rayo (t >= 0) ---

RelationCircleLine Circle::intersect(RayLine& r, Vect2d& pinter1, Vect2d& pinter2)
{
    Vect2d d(r.getB().getX() - r.getA().getX(), r.getB().getY() - r.getA().getY());
    Vect2d delta(r.getA().getX() - _center.getX(), r.getA().getY() - _center.getY());

    double dDotDelta = d.dot(delta);
    double dSq = d.dot(d);
    double deltaSq = delta.dot(delta);

    double disc = dDotDelta * dDotDelta - dSq * (deltaSq - _radius * _radius);

    if (BasicGeometry::equal(disc, 0.0))
    {
        double t = -dDotDelta / dSq;
        if (t >= 0.0)
        {
            pinter1 = Vect2d(r.getA()) + d.scalarMult(t);
            pinter2 = pinter1;
            return TANGENTS;
        }
        return NO_INTERSECT;
    }

    if (disc < 0.0)
        return NO_INTERSECT;

    int count = 0;



    double sqrtDisc = std::sqrt(disc);
    double t1 = (-dDotDelta - sqrtDisc) / dSq;
    double t2 = (-dDotDelta + sqrtDisc) / dSq;

    if (t1 >= 0.0)
    {
        pinter1 = Vect2d(r.getA()) + d.scalarMult(t1);
        count++;
    }

    if (t2 >= 0.0)
    {
        if (count == 0)
        {
            pinter1 = Vect2d(r.getA()) + d.scalarMult(t2);
        }
        else
        {
            pinter2 = Vect2d(r.getA()) + d.scalarMult(t2);
        }
        count++;
    }

    if (count == 0) return NO_INTERSECT;
    if (count == 1) { pinter2 = pinter1; return TANGENTS; }
    return INTERSECT;
}