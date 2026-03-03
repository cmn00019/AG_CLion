#include "stdafx.h"
#include "Plane.h"
#include "BasicGeometry.h"


Plane::Plane(Vect3d & p, Vect3d & u, Vect3d & v, bool arePoints)
{
	if (!arePoints)			// Vectors: pi = p + u * lambda + v * mu 
	{			
		_a = p;
		_b = u.add(_a);
		_c = v.add(_a);
	}
	else 
	{					// 3 points in the plane
		_a = p;
		_b = u;
		_c = v;
	}
}

Plane::Plane(const Plane & plane): _a(plane._a), _b(plane._b), _c(plane._c)
{
}

Plane::~Plane()
{
}

bool Plane::coplanar(Vect3d & point)
{  
    return BasicGeometry::equal(distance(point), 0.0);
}

double Plane::distance(Vect3d & point)
{
    // eq3d_dvp: lambda = -(n*p + d) / (n*n), dist = |lambda| * ||n||
    Vect3d n = getNormal();
    double d = getD();
    double nDotP = n.getX() * point.getX() + n.getY() * point.getY() + n.getZ() * point.getZ();
    double nDotN = n.dot(n);
    double lambda = -(nDotP + d) / nDotN;
    return std::abs(lambda) * n.module();
}    

double Plane::distance(Vect3d & v, Vect3d & q)
{
    // eq3d_dvp: lambda = -(n*v + d) / (n*n), q = v + lambda*n
    Vect3d n = getNormal();
    double d = getD();
    double nDotV = n.getX() * v.getX() + n.getY() * v.getY() + n.getZ() * v.getZ();
    double nDotN = n.dot(n);
    double lambda = -(nDotV + d) / nDotN;
    Vect3d lambdaN = n.scalarMul(lambda);
    q = v.add(lambdaN);
    return std::abs(lambda) * n.module();
}


double Plane::getA()
{
	return (BasicGeometry::determinant2x2(_c.getZ() - _a.getZ(), _c.getY() - _a.getY(), _b.getY() - _a.getY(), _b.getZ() - _a.getZ()));
}

double Plane::getB()
{
	return (BasicGeometry::determinant2x2(_c.getX() - _a.getX(), _c.getZ() - _a.getZ(), _b.getZ() - _a.getZ(), _b.getX() - _a.getX()));	
}

double Plane::getC()
{
	return (BasicGeometry::determinant2x2(_c.getY() - _a.getY(), _c.getX() - _a.getX(), _b.getX() - _a.getX(), _b.getY() - _a.getY()));
}

Vect3d Plane::getNormal()
{
    return Vect3d(getA(), getB(), getC());
}

bool Plane::intersect(Plane & plane, Line3d & line)
{
    // eq3d_ipp1: n3 = n1 x n2
    Vect3d n1 = this->getNormal();
    Vect3d n2 = plane.getNormal();
    Vect3d n3 = n1.xProduct(n2);

    if (BasicGeometry::equal(n3.module(), 0.0))
        return false; // Planos paralelos

    double d1 = this->getD();
    double d2 = plane.getD();


    double a1 = n1.getX(), b1 = n1.getY(), c1 = n1.getZ();
    double a2 = n2.getX(), b2 = n2.getY(), c2 = n2.getZ();
    double a3 = n3.getX(), b3 = n3.getY(), c3 = n3.getZ();
    double d3 = 0.0;

    double det = BasicGeometry::determinant3x3(a1, b1, c1, a2, b2, c2, a3, b3, c3);
    if (BasicGeometry::equal(det, 0.0))
        return false;

    double detX = BasicGeometry::determinant3x3(-d1, b1, c1, -d2, b2, c2, -d3, b3, c3);
    double detY = BasicGeometry::determinant3x3(a1, -d1, c1, a2, -d2, c2, a3, -d3, c3);
    double detZ = BasicGeometry::determinant3x3(a1, b1, -d1, a2, b2, -d2, a3, b3, -d3);

    Vect3d p0(detX / det, detY / det, detZ / det);
    Vect3d p1 = p0.add(n3);
    line = Line3d(p0, p1);
    return true;
}

bool Plane::intersect(Line3d & line, Vect3d & point)
{   
    // eq3d_ilp: lambda = -(n*t + d) / (n*v)
    Vect3d n = getNormal();
    double d = getD();
    Vect3d t = line.getOrigin();
    Vect3d dest = line.getDestination();
    Vect3d dir = dest.sub(t);

    double nDotV = n.dot(dir);

    if (BasicGeometry::equal(nDotV, 0.0))
        return false; // La Linea es paralela al plano

    double nDotT = n.dot(t);
    double lambda = -(nDotT + d) / nDotV;

    Vect3d lambdaV = dir.scalarMul(lambda);
    point = t.add(lambdaV);
    return true;
}

bool Plane::intersect(Plane& pa, Plane& pb, Vect3d& pinter)
{
    //Interseccion 3 planos
    double a1 = this->getA(), b1 = this->getB(), c1 = this->getC(), dd1 = this->getD();
    double a2 = pa.getA(), b2 = pa.getB(), c2 = pa.getC(), dd2 = pa.getD();
    double a3 = pb.getA(), b3 = pb.getB(), c3 = pb.getC(), dd3 = pb.getD();

    double det = BasicGeometry::determinant3x3(a1, b1, c1, a2, b2, c2, a3, b3, c3);
    if (BasicGeometry::equal(det, 0.0))
        return false;

    double detX = BasicGeometry::determinant3x3(-dd1, b1, c1, -dd2, b2, c2, -dd3, b3, c3);
    double detY = BasicGeometry::determinant3x3(a1, -dd1, c1, a2, -dd2, c2, a3, -dd3, c3);
    double detZ = BasicGeometry::determinant3x3(a1, b1, -dd1, a2, b2, -dd2, a3, b3, -dd3);

    pinter = Vect3d(detX / det, detY / det, detZ / det);
    return true;
}

Vect3d Plane::reflectedPoint(Vect3d & v)
{
    // eq3d_rvp: lambda = -2(n*p + d) / (n*n), q = p + lambda*n
    Vect3d n = getNormal();
    double d = getD();
    double nDotP = n.dot(v);
    double nDotN = n.dot(n);
    double lambda = -2.0 * (nDotP + d) / nDotN;
    Vect3d lambdaN = n.scalarMul(lambda);
    return v.add(lambdaN);
}

Plane & Plane::operator=(const Plane & plane)
{
	if (this != &plane)
	{
		_a = plane._a;
		_b = plane._b;
		_c = plane._c;
	}

	return *this;
}

std::ostream& operator<<(std::ostream& os, const Plane& plane)
{
	os << "Plane -> a: " << plane._a << ", b: " << plane._b << ", c: " << plane._c;
	return os;
}

// Helper: 2D point-in-polygon test (crossing number / ray casting)
static bool pointInPolygon2D(double px, double py, const std::vector<std::pair<double,double>>& poly)
{
    int n = (int)poly.size();
    int crossings = 0;
    for (int i = 0; i < n; i++)
    {
        double x1 = poly[i].first,  y1 = poly[i].second;
        double x2 = poly[(i+1) % n].first, y2 = poly[(i+1) % n].second;

        if ((y1 <= py && y2 > py) || (y2 <= py && y1 > py))
        {
            double t = (py - y1) / (y2 - y1);
            if (px < x1 + t * (x2 - x1))
                crossings++;
        }
    }
    return (crossings % 2) != 0;
}

bool Plane::intersectLine3dPolygon(Line3d& line, std::vector<Vect3d>& polygon, Vect3d& pinter)
{
    if (polygon.size() < 3)
        return false;

    // Build the polygon's plane from first 3 vertices
    Plane polyPlane(polygon[0], polygon[1], polygon[2], true);

    // Intersect line with plane
    if (!polyPlane.intersect(line, pinter))
        return false;

    // Project to 2D by dropping the coordinate with largest normal component
    Vect3d n = polyPlane.getNormal();
    double ax = std::abs(n.getX()), ay = std::abs(n.getY()), az = std::abs(n.getZ());

    // Determine which axis to drop
    int dropAxis = 2; // drop Z by default
    if (ax >= ay && ax >= az) dropAxis = 0; // drop X
    else if (ay >= ax && ay >= az) dropAxis = 1; // drop Y

    // Project polygon vertices and intersection point to 2D
    auto project2D = [dropAxis](Vect3d& v) -> std::pair<double,double> {
        if (dropAxis == 0) return {v.getY(), v.getZ()};
        if (dropAxis == 1) return {v.getX(), v.getZ()};
        return {v.getX(), v.getY()};
    };

    std::vector<std::pair<double,double>> poly2D;
    for (auto& v : polygon)
        poly2D.push_back(project2D(v));

    auto pt2D = project2D(pinter);

    return pointInPolygon2D(pt2D.first, pt2D.second, poly2D);
}

bool Plane::intersectSegment3dPolygon(Segment3d& segment, std::vector<Vect3d>& polygon, Vect3d& pinter)
{
    if (polygon.size() < 3)
        return false;

    // Build the polygon's plane from first 3 vertices
    Plane polyPlane(polygon[0], polygon[1], polygon[2], true);

    // Intersect as a line first
    Vect3d orig = segment.getOrigin();
    Vect3d dest = segment.getDestination();
    Line3d asLine(orig, dest);

    if (!polyPlane.intersect(asLine, pinter))
        return false;

    // Check that the intersection point is within segment bounds (0 <= t <= 1)
    Vect3d dir = dest.sub(orig);
    double dirDot = dir.dot(dir);
    if (BasicGeometry::equal(dirDot, 0.0))
        return false;

    Vect3d toInter = pinter.sub(orig);
    double t = dir.dot(toInter) / dirDot;
    if (t < 0.0 || t > 1.0)
        return false;

    // Project to 2D and do point-in-polygon test
    Vect3d n = polyPlane.getNormal();
    double ax = std::abs(n.getX()), ay = std::abs(n.getY()), az = std::abs(n.getZ());

    int dropAxis = 2;
    if (ax >= ay && ax >= az) dropAxis = 0;
    else if (ay >= ax && ay >= az) dropAxis = 1;

    auto project2D = [dropAxis](Vect3d& v) -> std::pair<double,double> {
        if (dropAxis == 0) return {v.getY(), v.getZ()};
        if (dropAxis == 1) return {v.getX(), v.getZ()};
        return {v.getX(), v.getY()};
    };

    std::vector<std::pair<double,double>> poly2D;
    for (auto& v : polygon)
        poly2D.push_back(project2D(v));

    auto pt2D = project2D(pinter);

    return pointInPolygon2D(pt2D.first, pt2D.second, poly2D);
}