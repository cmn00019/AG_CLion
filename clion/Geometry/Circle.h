#pragma once

/* 
 *  File:   Circle.h
 *  Author: lidia
 *
 *  Created on 8 de febrero de 2021, 19:32
 */

#include "Point.h"
#include "Polygon.h"
#include "Line.h"
#include "RayLine.h"
#include "SegmentLine.h"

enum RelationCircles { CONCENTRIC, EXTERNAL, INTERNAL, SECANT, INTERIOR_TANG, EXTERNAL_TANG };
enum RelationCircleLine { INTERSECT, TANGENTS, NO_INTERSECT };

class Circle 
{
protected:
    Point       _center;             
    double      _radius;

public:
    /**
	*	@brief Default constructor.
	*/
    Circle (): _center(Point (0,0)), _radius(1.0) { }

    /**
    *	@brief Constructor.
	*/
    Circle(const Point &center, double radius): _center(center), _radius(radius) { }

    /**
	*	@brief Copy constructor.
	*/
    Circle (const Circle& orig): _center(orig._center), _radius(orig._radius) { }
    
	/**
	*	@brief Destructor.
	*/
    virtual ~Circle();

    /**
    *   @brief Checks if the point is inside the circle.
    */  
    bool isInside (const Point &p);
    
    /**
    *   @return A polygon with circular shape.
    */
    Polygon getPointsCircle (uint32_t numSubdivisions = 256);

    /**
    *	@brief Assignment operator (override).
    */
    virtual Circle& operator=(const Circle& circle);

    /**
    *   @brief Determines the relationship between this circle and another.
    */
    RelationCircles relacionaCir(Circle& c);

    /**
    *   @brief Determines the relationship between this circle and a line.
    */
    RelationCircleLine relacionaLine(Line& l);

    /**
    *   @brief Intersects this circle with a line.
    */
    RelationCircleLine intersect(Line& l, Vect2d& pinter1, Vect2d& pinter2);

    /**
    *   @brief Intersects this circle with a segment (0<=t<=1).
    */
    RelationCircleLine intersect(SegmentLine& s, Vect2d& pinter1, Vect2d& pinter2);

    /**
    *   @brief Intersects this circle with a ray (t>=0).
    */
    RelationCircleLine intersect(RayLine& r, Vect2d& pinter1, Vect2d& pinter2);
};
