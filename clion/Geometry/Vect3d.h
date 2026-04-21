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
 * File:   Vect3d.h
 * Author: lidia
 *
 * Created on 25 de enero de 2021, 19:11
 */

#pragma once

/**
*	@brief Base class for any structure which needs 3 coordinates (point, vector...).
*	@author Lidia Mª Ortega Alvarado.
*/
class Vect3d
{
protected:
	enum Axes { X, Y, Z };

private:
	double _value[3];

public:
	/**
	*	@brief Default constructor.
	*/
	Vect3d();

	/**
	*	@brief Constructor.
	*/
	Vect3d(double x, double y, double z);

	/**
	*	@brief Copy constructor.
	*/
	Vect3d(const Vect3d& vector);

	/**
	*	@brief Destructor.
	*/
	virtual ~Vect3d();

	/**
	*	@brief Vector sum. a + b.
	*/
	Vect3d add(const Vect3d& b) const;

	/**
	*	@brief Checks if a, b and this vector are on a same line.
	*/
	bool collinear(const Vect3d& a, const Vect3d& b) const;

	/**
	*	@brief Distance between points.
	*/
	double distance(const Vect3d& p) const;

	/**
	*	@brief Dot product.
	*/
	double dot(const Vect3d& v) const;

	/**
	*	@brief Returns the coordinates of this vector as an array.
	*/
	std::vector<double> getVert() const;

	/**
	*	@brief Returns the equidistant plane between this point and v.
	*	@param v The other point.
	*	@param n Output normal vector.
	*	@param d Output independent term.
	*/
	void getPlane(const Vect3d& v, Vect3d& n, float& d) const;

	/**
	*	@brief Returns a certain coordinate of the vector.
	*/
	double get(unsigned int index) const { return _value[index]; }

	/**
	*	@brief Returns the X coordinate.
	*/
	double getX() const;

	/**
	*	@brief Returns the Y coordinate.
	*/
	double getY() const;

	/**
	*	@brief Returns the Z coordinate.
	*/
	double getZ() const;

	/**
	*	@brief Returns the module of the vector.
	*/
	double module() const;

	/**
	*	@brief Assignment operator.
	*/
	virtual Vect3d& operator=(const Vect3d& vector);

	/**
	*	@brief Checks if two vectors are equal.
	*/
	virtual bool operator==(const Vect3d& vector) const;

	/**
	*	@brief Checks if two vectors are distinct.
	*/
	virtual bool operator!=(const Vect3d& vector) const;

	/**
	*	@brief Shows some information of the vector as debugging info.
	*/
	friend std::ostream& operator<<(std::ostream& os, const Vect3d& vec);

	/**
	*	@brief Multiplication of this vector by an scalar value.
	*/
	Vect3d scalarMul(double value) const;

	/**
	*	@brief Modifies the value of a certain coordinate.
	*/
	void set(unsigned int index, double value) { _value[index] = value; }

	/**
	*	@brief Modifies the X coordinate.
	*/
	void setX(double x);

	/**
	*	@brief Modifies the Y coordinate.
	*/
	void setY(double y);

	/**
	*	@brief Modifies the Z coordinate.
	*/
	void setZ(double z);

	/**
	*	@brief Modifies all the vector values.
	*/
	void setVert(double x, double y, double z);

	/**
	*	@brief Vector subtraction.
	*/
	Vect3d sub(const Vect3d& b) const;

	/**
	*	@brief Vectorial product.
	*/
	Vect3d xProduct(const Vect3d& b) const;
};

