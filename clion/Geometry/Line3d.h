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
 * File:   Line3d.h
 * Author: lidia
 *
 * Created on 25 de enero de 2021, 19:55
 */

#ifndef LINE3D_H
#define LINE3D_H

#include "Edge3d.h"
#include "Line3d.h"


class Line3d : public Edge3d
{
public:
	enum classifyLines { NON_INTERSECT, PARALLEL, INTERSECT, THESAME };

protected:
	/**
	*	@brief Checks if the parametric value t is valid. Any value is valid for a line.
	*/
	virtual bool isTvalid(double t) const { return true; }

public:
	/**
	*	@brief Default constructor.
	*/
	Line3d();

	/**
	*	@brief Constructor.
	*/
	Line3d(const Vect3d& orig, const Vect3d& dest);

	/**
	*	@brief Copy constructor.
	*/
	Line3d(const Line3d& line);

	/**
	*	@brief Destructor.
	*/
	virtual ~Line3d();

	/**
	*	@brief Distance between two lines.
	*/
	double distance(const Line3d& line) const;
    
    
    /**
    *    @brief Distance between point p and this.
    */
    double distance(const Vect3d& p) const;

	/**
	*	@brief Returns the normal to this line that passes through p.
	*/
	Line3d normalLine(const Vect3d& p) const;

	/**
	*	@brief Assignment operator.
	*/
	virtual Line3d& operator=(const Line3d& line);

	/**
	*	@brief Shows line data at the debug window.
	*/
	friend std::ostream& operator<<(std::ostream& os, const Line3d& line);
};




#endif /* LINE3D_H */

