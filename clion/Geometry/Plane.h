/*   Copyright (C) 2023 Lidia Ortega Alvarado, Alfonso Lopez Ruiz
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see https://github.com/AlfonsoLRz/AG2223.
*/

#pragma once

/* 
 * File:   Plane.h
 * Author: lidia
 *
 * Created on 26 de enero de 2021, 17:56
 */

#include "Line3d.h"
#include "Segment3d.h"
#include "Vect3d.h"

/**
*	@brief This class a represents a 3D plane represented by three points.
*	@author Lidia Mª Ortega Alvarado.
*/
class Plane
{
public:
	enum IntersectionType 
	{
		POINT, SEGMENT, COPLANAR
	};

public:
	class IntersectionLine 
	{
		Vect3d _point;
		IntersectionType _type;
	};

protected:
	Vect3d _a, _b, _c;  

public:
	/**
	*
	*	@param p in pi = p + u * lambda + v * mu -> r from the points (R, S, T).
	*	@param u in pi = p + u * lambda + v * mu -> d from the points (R, S, T).
	*	@param v in pi = p + u * lambda + v * mu -> t from the points (R, S, T).
	*	@param If arePoints is false, then params are p + u * lambda + v * mu, otherwise are points (R, S, T).
	*/
	Plane(const Vect3d& p, const Vect3d& u, const Vect3d& v, bool hayPuntos);

	/**
	*	@brief Copy constructor.
	*/
	Plane(const Plane& plane);

	/**
	*	@brief Destructor.
	*/
	virtual ~Plane();

	/**
	*	@brief Returns true if p is in the plane.
	*/	
	bool coplanar(const Vect3d& point) const;

	/**
	*	@brief Distance between the plane and the point.
	*/
	double distance(const Vect3d& point) const;

	/**
	*	@brief Distance between the plane and the point v, also returns the closest point q on the plane. (eq3d_dvp)
	*/
	double distance(const Vect3d& v, Vect3d& q) const;

	/**
	*	@brief Returns A in AX + BY + CZ + D = 0.
	*/
	double getA() const;

	/**
	*	@brief Returns B in AX + BY + CZ + D = 0.
	*/
	double getB() const;

	/**
	*	@brief Returns C in AX + BY + CZ + D = 0.
	*/
	double getC() const;

	/**
	*	@return D in AX + BY + CZ + D = 0.
	*/
	double getD() const { return (-1.0) * (getA() * _a.getX() + getB() * _a.getY() + getC() * _a.getZ()); }
	Vect3d getNormal() const;

    /**
	*	@brief Calculates the intersection point of a line and this plane, if exists.
	*/
	bool intersect(const Line3d& line, Vect3d& point) const;
        
    /**
	*	@brief Calculates the intersection point of three planes.
	*/
	bool intersect(const Plane& pa, const Plane& pb, Vect3d& punto) const;
        
	/**
	*	@brief Calculates the intersection line of a plane with this plane.
	*/
	bool intersect(const Plane& plane, Line3d& line) const;

	/**
	*	@brief Reflects a point with respect to this plane.
	*/
	Vect3d reflectedPoint(const Vect3d& v) const;

	/**
	*	@brief Intersection of a line with a 3D polygon (sorteo #6).
	*/
	static bool intersectLine3dPolygon(const Line3d& line, const std::vector<Vect3d>& polygon, Vect3d& punto);

	/**
	*	@brief Intersection of a segment with a 3D polygon (sorteo #8).
	*/
	static bool intersectSegment3dPolygon(const Segment3d& segment, const std::vector<Vect3d>& polygon, Vect3d& punto);
	
	/**
	*	@brief Assignment operator.
	*/
	virtual Plane& operator=(const Plane& plane);

	/**
	*	@brief Shows the plane values at the debug window.
	*/
	friend std::ostream& operator<<(std::ostream& os, const Plane& plane);
};

