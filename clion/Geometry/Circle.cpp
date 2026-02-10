#include "stdafx.h"
#include "Circle.h"

// Public methods

Circle::~Circle()
{
}

bool Circle::isInside(const Point &p)
{ 
    // XXXX
    return true;
}

Polygon Circle::getPointsCircle(uint32_t numSubdivisions)
{        
    Polygon polygon;
    float angle = .0f, alpha = 2.0f * glm::pi<float>() / numSubdivisions;

    while (angle < (2.0f * glm::pi<float>() + glm::epsilon<float>()))
    {
        Point point = Point(std::cos(angle), -std::sin(angle));
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
    
  
   