#include "stdafx.h"
#include "AABB.h"
#include "Triangle3d.h"
#include "Ray3d.h"

// Métodos públicos

AABB::AABB(const vec3& min, const vec3& max) : _max(max), _min(min)
{
}

AABB::AABB(const AABB& aabb) : _max(aabb._max), _min(aabb._min)
{
}

AABB::~AABB()
{
}

AABB& AABB::operator=(const AABB& aabb)
{
	_max = aabb._max;
	_min = aabb._min;

	return *this;
}

AABB AABB::dot(const mat4& matrix)
{
	return AABB(matrix * vec4(_min, 1.0f), matrix * vec4(_max, 1.0f));
}

bool AABB::AABB_tri(Triangle3d& t)
{
	return t.tri_AABB(*this);
}

void AABB::update(const AABB& aabb)
{
	this->update(aabb.max());
	this->update(aabb.min());
}

void AABB::update(const vec3& point)
{
	if (point.x < _min.x) { _min.x = point.x; }
	if (point.y < _min.y) { _min.y = point.y; }
	if (point.z < _min.z) { _min.z = point.z; }

	if (point.x > _max.x) { _max.x = point.x; }
	if (point.y > _max.y) { _max.y = point.y; }
	if (point.z > _max.z) { _max.z = point.z; }
}

std::ostream& operator<<(std::ostream& os, const AABB& aabb)
{
	os << "Maximum corner: " << aabb.max().x << ", " << aabb.max().y << ", " << aabb.max().z << "\n";
	os << "Minimum corner: " << aabb.min().x << ", " << aabb.min().y << ", " << aabb.min().z << "\n";

	return os;
}

bool AABB::rayIntersects(Ray3d& ray)
{
    vec3 origin(ray.getOrigin().getX(), ray.getOrigin().getY(), ray.getOrigin().getZ());
    vec3 dest(ray.getDestination().getX(), ray.getDestination().getY(), ray.getDestination().getZ());
    vec3 dir = dest - origin;
    
    float len = glm::length(dir);
    if (len < 0.0001f) return false;
    dir /= len;
    
    vec3 invDir(1.0f / dir.x, 1.0f / dir.y, 1.0f / dir.z);
    
    float t1 = (_min.x - origin.x) * invDir.x;
    float t2 = (_max.x - origin.x) * invDir.x;
    float t3 = (_min.y - origin.y) * invDir.y;
    float t4 = (_max.y - origin.y) * invDir.y;
    float t5 = (_min.z - origin.z) * invDir.z;
    float t6 = (_max.z - origin.z) * invDir.z;

    float tmin = glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)), glm::min(t5, t6));
    float tmax = glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)), glm::max(t5, t6));

    if (tmax < 0) return false;
    if (tmin > tmax) return false;

    return true;
}
