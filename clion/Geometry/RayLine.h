#pragma once

/* 
 *	File:   RayLine.h
 *	Author: lidia
 *
 *	Created on 19 de enero de 2021, 10:12
 */

#include "SegmentLine.h"

class RayLine : public SegmentLine
{
public:
	/**
	*	@brief Constructor of a rayline given its two points.
	*/
	RayLine(const Point& a, const Point& b);

	/**
	*	@brief Copy constructor.
	*/
	RayLine(const RayLine& s);

	/**
	*	@brief Destructor.
	*/
	~RayLine() override;

	/**
	*	@brief Distance from a point defined by 'vector' to this rayline.
	*/
	float distanceToPoint(Vect2d& vector);

	/**
	*	@brief Checks if this rayline intersects with a segment in an incorrect way.
	*/
	bool incorrectSegmentIntersection(SegmentLine& segment);

	/**
	*	@brief Checks if a rayline intersects with a line.
	*	@param intersection If rayline and line intersect, then this point is the intersection. Otherwise this point is not valid.
	*/
	//virtual bool intersects(Line& line, Vect2d& intersetion);

	/**
	*	@brief Checks if a rayline intersects with this one.
	*	@param intersection If both raylines intersect, then this point is the intersection. Otherwise this point is not valid.
	*/
	//virtual bool intersects(RayLine& rayline, Vect2d& intersetion);

	/**
	*	@brief Checks if a segment intersects with a segment. 
	*	@param intersection If rayline and segment intersect, then this point is the intersection. Otherwise this point is not valid.
	*/
	//virtual bool intersects(SegmentLine& segment, Vect2d& intersetion);

	/**
	*	@brief Checks if the parameter t is valid to get a point of the rayline.
	*/
	bool isTvalid(double t) override { return t >= 0; }

	/**
	*	@brief Assignment operator.
	*/
	virtual RayLine& operator=(const RayLine& rayline);

	/**
	*	@brief Overriding cout call.
	*/
	friend std::ostream& operator<<(std::ostream& os, const RayLine& ray);
};

