#pragma once

/* 
 * File:   SegmentLIne.h
 * Author: lidia
 *
 * Created on 17 de enero de 2021, 17:49
 */

#include "Point.h"
#include "Vect2d.h"

class Line;
class RayLine; 

class SegmentLine 
{    
	friend class DrawSegment;        

protected:      
    Point	_orig, _dest; 

protected:
    /**
    *	@brief Returns the parametric value T0 to calculate the distance between a point and any geometric object like lines, segments or raylines.
    */
    float getDistanceT0(Vect2d& point);
    
	/**
	*	@brief Obtains the parameters s and t where both lines intersect, if they do.
	*	@param c Origin of the second line (C).
	*	@param d Destination of the second line (D).
	*	@param s Parameter for this segment (AB).
	*	@param t Parameter for the other line (CD).
	*	@return true if lines are not parallel and s is in [0,1].
	*/
    virtual bool intersects(Vect2d& c, Vect2d& d, double& s, double& t);

public:    
    /**
	*	@brief Default constructor.
	*/
	SegmentLine();

	/**
	*	@brief Constructor.
	*/
	SegmentLine(const Point& a, const Point& b);   
        
    /**
	*	@brief Copy constructor.
	*/
	SegmentLine(const SegmentLine& segment);
               
	/**
	*	@brief Constructor.
	*/
	SegmentLine(double ax, double ay, double bx, double by);

	/**
	*	@brief Destructor.
	*/
	virtual ~SegmentLine();
  
	/**
	*	@brief Returns the origin of the segment.
	*/
	Point &getA() { return _orig; }

	/**
	*	@brief Returns the end of the segment.
	*/
	Point &getB() { return _dest; }

	/**
	*	@brief Returns the constant of the equation of the implied line: c = y-mx.
	*/
	double getC();
        
    /**
	*	@brief Checks if a segment is different to this one.
	*/
	bool distinct(SegmentLine& segment);
      
	/**
	*	@brief Distance from a point defined by 'vector' to this segment.
	*/
	double distPointSegment(Vect2d& vector);

	/**
	*	@brief Checks if this segment intersects with a line.
	*	@param intersection Point of intersection if it exists.
	*/
	bool intersects(Line& r, Vect2d& res);

	/**
	*	@brief Checks if this segment intersects with a ray.
	*	@param intersection Point of intersection if it exists.
	*/
	bool intersects(RayLine& r, Vect2d& res);

	/**
	*	@brief Checks if this segment intersects with another segment.
	*	@param intersection Point of intersection if it exists.
	*/
	bool intersects(SegmentLine& r, Vect2d& res);

	/**
	*	@brief Checks if a segment is equal to this one.
	*/
	bool equal(SegmentLine& segment);

	/**
	*	@brief It obtains the point belonging to the segment or colineal to it for a concrete t in the parametric equation: result = a + t (b-a).
	*/
	Point getPoint(double t);

	/**
	*	@brief Determines whether two segments intersect improperly, that is, when one end of a segment is contained in the other. Use integer arithmetic.
	*/
	bool impSegmentIntersection(SegmentLine& segment);

	/**
	*	@brief Determines whether a segment is horizontal or not (using EPSILON).
	*/
	bool isHorizontal();   
	
	/**
	*	@brief Determines whether or not a segment is vertical (using EPSILON).
	*/
	bool isVertical();

	/**
	*	@brief Check if the parameter t is valid to get a point of the segment.
	*/
	virtual bool isTvalid(double t) { return ((t >= 0) && (t <= 1)); }

	/**
	*	@brief Determines whether p is in the left of SegmentLine.
	*/
	bool left(Point& p) { return p.left(_orig, _dest); }

	/**
	*	@brief Returns the length of the segment.
	*/
	double length() { return _orig.distance(_dest); }

	/**
	*	@brief Assignment operator.
	*/
	virtual SegmentLine& operator=(const SegmentLine& segment);

	/**
	*	@brief Overriding cout call.
	*/
	friend std::ostream& operator<<(std::ostream& os, const SegmentLine& segment);

	/**
	*	@brief Determines whether two segments intersect in their own way, that is, when they intersect completely. Use only arithmetic.
	*/
	virtual bool segmentIntersection(SegmentLine& l);

	/**
	*	@brief Modifies the origin of the segment.
	*/
	void setA(Point& p) { _orig = p; }

	/**
	*	@brief Returns the slope of the implied straight line equation: m = (yb-ya) / (xb-xa).
	*/
	double slope();

	/**
	*	@brief Returns the area formed by the triangle composed of the current SegmentLine and the union of its bounds with p.
	*/
	double triangleArea2(Point& p) { return p.triangleArea2(_orig, _dest); }      
};

