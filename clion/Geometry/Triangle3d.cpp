#include "stdafx.h"
#include "BasicGeometry.h"
#include "Triangle3d.h"
#include "Edge3d.h"
#include "Ray3d.h"
#include "AABB.h"
#include <algorithm>


Triangle3d::Triangle3d()
{
}

Triangle3d::Triangle3d(double ax, double ay, double az, double bx, double by, double bz, double cx, double cy, double cz)
	: _a(ax, ay, az), _b(bx, by, bz), _c(cx, cy, cz)
{
}

Triangle3d::Triangle3d(const Triangle3d & triangle)
	: _a(triangle._a), _b(triangle._b), _c(triangle._c)
{
}

Triangle3d::Triangle3d(Vect3d & va, Vect3d & vb, Vect3d & vc)
	: _a(va), _b(vb), _c(vc)
{
}

Triangle3d::~Triangle3d()
{
}

double Triangle3d::area()
{
    Vect3d v1(_b.sub(_a));
    Vect3d v2(_c.sub(_a));
    Vect3d cross = v1.xProduct(v2);
    return cross.module() / 2.0;
}

bool Triangle3d::ray_tri(Ray3d& ray, Vect3d& p)
{
    // Algoritmo de Moller-Trumbore
    Vect3d edge1(_b.sub(_a));
    Vect3d edge2(_c.sub(_a));
    
    Vect3d origin = ray.getOrigin();
    Vect3d dest = ray.getDestination();
    Vect3d rayDir = dest.sub(origin); 
    Vect3d h = rayDir.xProduct(edge2);
    double a = edge1.dot(h);
    
    if (a > -glm::epsilon<double>() && a < glm::epsilon<double>()) 
        return false;
        
    double f = 1.0 / a;
    Vect3d s = origin.sub(_a);
    double u = f * s.dot(h);
    
    if (u < 0.0 || u > 1.0)
        return false;
        
    Vect3d q = s.xProduct(edge1);
    double v = f * rayDir.dot(q);
    
    if (v < 0.0 || u + v > 1.0)
        return false;
        
    double t = f * edge2.dot(q);
    if (t > glm::epsilon<double>()) 
    {
        Vect3d move = rayDir.scalarMul(t);
        p = origin.add(move);
        return true;
    }
    
    return false;
}

bool Triangle3d::tri_AABB(AABB& box)
{
    vec3 c = box.center();
    vec3 e = box.extent();
    
    vec3 v0(_a.getX() - c.x, _a.getY() - c.y, _a.getZ() - c.z);
    vec3 v1(_b.getX() - c.x, _b.getY() - c.y, _b.getZ() - c.z);
    vec3 v2(_c.getX() - c.x, _c.getY() - c.y, _c.getZ() - c.z);
    
    vec3 f0 = v1 - v0;
    vec3 f1 = v2 - v1;
    vec3 f2 = v0 - v2;
    
    vec3 u[3] = { vec3(1.0f,0.0f,0.0f), vec3(0.0f,1.0f,0.0f), vec3(0.0f,0.0f,1.0f) };
    vec3 f[3] = { f0, f1, f2 };
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            vec3 axis = glm::cross(u[i], f[j]);
            
            float p0 = glm::dot(v0, axis);
            float p1 = glm::dot(v1, axis);
            float p2 = glm::dot(v2, axis);
            float r = e.x * std::abs(glm::dot(u[0], axis)) +
                      e.y * std::abs(glm::dot(u[1], axis)) +
                      e.z * std::abs(glm::dot(u[2], axis));
            
            float p_min = std::min({p0, p1, p2});
            float p_max = std::max({p0, p1, p2});
            
            if (p_min > r || p_max < -r)
                return false;
        }
    }
    
    for (int i = 0; i < 3; i++) {
        float p0 = glm::dot(v0, u[i]);
        float p1 = glm::dot(v1, u[i]);
        float p2 = glm::dot(v2, u[i]);
        float p_min = std::min({p0, p1, p2});
        float p_max = std::max({p0, p1, p2});
        
        if (p_min > e[i] || p_max < -e[i]) return false;
    }
    
    vec3 normal = glm::cross(f0, f1);
    float p0 = glm::dot(v0, normal);
    float r = e.x * std::abs(normal.x) + e.y * std::abs(normal.y) + e.z * std::abs(normal.z);
    
    if (p0 > r || p0 < -r)
        return false;
        
    return true;
}

Triangle3d::PointPosition Triangle3d::classify(Vect3d & point)
{
    Vect3d n = this->normal();
    Vect3d diff = point.sub(_a);
    double d = n.dot(diff);

    if (BasicGeometry::equal(d, 0.0))
        return PointPosition::COPLANAR;
    if (d > 0.0)
        return PointPosition::POSITIVE;
    return PointPosition::NEGATIVE;
}

Vect3d Triangle3d::normal()
{
	Vect3d v1 (_b.sub(_a));
	Vect3d v2 (_c.sub(_a));
	Vect3d n (v1.xProduct(v2));
	double module = n.module();

	return (n.scalarMul(1.0f / module));
}

Triangle3d & Triangle3d::operator=(const Triangle3d & triangle)
{
	_a = triangle._a;
	_b = triangle._b;
	_c = triangle._c;

	return *this;
}

std::ostream& operator<<(std::ostream& os, const Triangle3d& triangle)
{
	std::cout << "Triangulo 3D -> " << triangle._a << ", " << triangle._b << ", " << triangle._c;
	return os;
}

void Triangle3d::set(Vect3d & va, Vect3d & vb, Vect3d & vc)
{
	_a = va;
	_b = vb;
	_c = vc;
}

