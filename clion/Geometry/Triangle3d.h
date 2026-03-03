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


/* 
 * File:   Triangle3D.h
 * Author: lidia
 *
 * Created on 25 de enero de 2021, 19:14
 */

#pragma once

#include "Vect3d.h"

/**
*	@brief This class represents a triangle defined by 3 points.
*	@author Lidia Mª Ortega Alvarado.
*/
class Triangle3d
{
public:
    
    friend class DrawTriangle3d;  
    
	enum PointPosition 
	{
		POSITIVE, NEGATIVE, COPLANAR
	};

	enum PointTrianglePosition 
	{
		PARALELL, COLLINEAR, INTERSECTS, NO_INTERSECTS
	};

	// Contains all those attributes necessary for resolving the intersection between a triangle and an AABB.
	struct TriangleAABBIntersData
	{
		Vect3d	_v0, _v1, _v2;
		float	_min, _max, _rad;
		Vect3d	_normal, _edge0, _edge1, _edge2, _p, _fedge;
		Vect3d	_boxCenter;
		Vect3d	_boxRadius;
	};

protected:
	Vect3d _a, _b, _c;

public:
	/**
	*	@brief Default constructor.
	*/
	Triangle3d();

	/**
	*	@brief Constructor.
	*/
	Triangle3d(double ax, double ay, double az, double bx, double by, double bz, double cx, double cy, double cz);

	/**
	*	@brief Copy constructor.
	*/
	Triangle3d(const Triangle3d& triangle);

	/**
	*	@brief Constructor.
	*/
	Triangle3d(Vect3d& va, Vect3d& vb, Vect3d& vc);

	/**
	*	@brief Destructor.
	*/
	virtual ~Triangle3d();

	/**
	*	@brief Returns the are of the triangle.
	*/
	double area();

	/**
	*	@brief Returns the position of the point respect to the triangle plane.
	*/
	PointPosition classify(Vect3d& point);

	/**
	*	@brief Returns a new triangle with the same values than this one.
	*/
	Triangle3d copy() { return Triangle3d(_a, _b, _c); }

	/**
	*	@brief Returns the first point.
	*/
	Vect3d getA() { return _a; }

	/**
	*	@brief Returns the second point.
	*/
	Vect3d getB() { return _b; }
	
	/**
	*	@brief Returns the third point.
	*/
	Vect3d getC() { return _c; }

	/**
	*	@brief Returns the normal of the triangle.
	*/
	Vect3d normal();

	/**
	*	@brief Assignment operator.
	*/
	virtual Triangle3d& operator=(const Triangle3d& triangle);

	/**
	*	@brief Cout overloading.
	*/
	friend std::ostream& operator<<(std::ostream& os, const Triangle3d& triangle);

	/**
	*	@brief Modifies the first point.
	*/
	void setA(Vect3d& pa) { _a = pa; }

	/**
	*	@brief Modifies the second point.
	*/
	void setB(Vect3d& pb) { _b = pb; }

	/**
	*	@brief Modifies the third point.
	*/
	void setC(Vect3d& pc) { _c = pc; }

	/**
	*	@brief Modifies all the points.
	*/
	void set(Vect3d& va, Vect3d& vb, Vect3d& vc);	
};

