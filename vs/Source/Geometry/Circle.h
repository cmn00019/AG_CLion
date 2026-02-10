#pragma once

/* 
 *  File:   Circle.h
 *  Author: lidia
 *
 *  Created on 8 de febrero de 2021, 19:32
 */

#include "Point.h"
#include "Polygon.h"


class Circle 
{
protected:
    Point       _center;             
    double      _radius;

public:
    /**
	*	@brief Default constructor. Coordinates are initialized with an invalid value.
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
    *   @param p
    *   @return true if p is inside the circle 
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
};

