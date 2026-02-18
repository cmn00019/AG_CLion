#pragma once

/* 
 *	File:   Line.h
 *	Author: lidia
 *
 *	Created on 18 de enero de 2021, 19:39
 */

#include "SegmentLine.h"

class Line : public SegmentLine
{
protected:
	/**
	*	@brief Override: for a Line, s has no restriction (line is infinite).
	*/
	bool intersects(Vect2d& c, Vect2d& d, double& s, double& t) override;

public:
	/**
	*	@brief Constructor.
	*/
	Line(const Point& a, const Point& b);

	/**
	*	@brief Copy constructor.
	*/
	Line(const Line& s);

	/**
	*	@brief Destructor.
	*/
	~Line() override;

	/**
	*	@brief Distance from a point defined by 'vector' to this line.
	*/
	double distancePointLine(Vect2d& v);

	/**
	*	@brief Checks if the specified line intersects with this one.
	*	@param intersection If both lines intersect, then this point is the intersection. Otherwise this point is not valid.
	*/
	virtual bool intersects(Line& line, Vect2d& intersection);

	/**
	*	@brief Checks if the specified rayline intersects with this line.
	*	@param intersection If rayline and line intersect, then this point is the intersection. Otherwise this point is not valid.
	*/
	virtual bool intersects(RayLine& rayline, Vect2d& intersection);

	/**
	*	@brief Checks if the specified segment intersects with this line.
	*	@param intersection If line and segment intersect, then this point is the intersection. Otherwise this point is not valid.
	*/
	virtual bool intersects(SegmentLine& segment, Vect2d& intersection);

	/**
	*	@brief Checks if a segment line generates an incorrect intersection.
	*/
	bool incorrectSegmentIntersection(SegmentLine& l);

	/**
	*	@brief Checks if the parameter t is valid to obtain a point for a line (it's always valid, we just need to override).
	*/
	bool isTvalid(double t) override { return true; }

	/**
	*	@brief Assignment operator.
	*/
	Line& operator=(const Line& line);

	/**
	*	@brief Overriding cout call.
	*/
	friend std::ostream& operator<<(std::ostream& os, const Line& line);

	/**
	*	@brief Checks if a segment intersects with this line (proper intersection).
	*/
	bool segmentIntersection(SegmentLine& l) override;
};

