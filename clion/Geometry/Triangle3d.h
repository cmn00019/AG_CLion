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

class Ray3d;
class AABB;

/**
*	@brief Esta clase representa un triangulo definido por 3 puntos.
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

	// Contiene todos los atributos necesarios para resolver la interseccion entre un triangulo y un AABB.
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
	*	@brief Constructor por defecto.
	*/
	Triangle3d();

	/**
	*	@brief Constructor.
	*/
	Triangle3d(double ax, double ay, double az, double bx, double by, double bz, double cx, double cy, double cz);

	/**
	*	@brief Constructor de copia.
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
	*	@brief Devuelve el area del triangulo.
	*/
	double area();

	/**
	*	@brief Prueba de interseccion Rayo-Triangulo.
	*/
	bool ray_tri(Ray3d& ray, Vect3d& p);

	/**
	*	@brief Prueba de interseccion AABB-Triangulo.
	*/
	bool tri_AABB(AABB& box);

	/**
	*	@brief Devuelve la posicion del punto respecto al plano del triangulo.
	*/
	PointPosition classify(Vect3d& point);

	/**
	*	@brief Devuelve un nuevo triangulo con los mismos valores que este.
	*/
	Triangle3d copy() { return Triangle3d(_a, _b, _c); }

	/**
	*	@brief Devuelve el primer punto.
	*/
	Vect3d getA() { return _a; }

	/**
	*	@brief Devuelve el segundo punto.
	*/
	Vect3d getB() { return _b; }
	
	/**
	*	@brief Devuelve el tercer punto.
	*/
	Vect3d getC() { return _c; }

	/**
	*	@brief Devuelve la normal del triangulo.
	*/
	Vect3d normal();

	/**
	*	@brief Operador de asignacion.
	*/
	virtual Triangle3d& operator=(const Triangle3d& triangle);

	/**
	*	@brief Sobrecarga de Cout.
	*/
	friend std::ostream& operator<<(std::ostream& os, const Triangle3d& triangle);

	/**
	*	@brief Modifica el primer punto.
	*/
	void setA(Vect3d& pa) { _a = pa; }

	/**
	*	@brief Modifica el segundo punto.
	*/
	void setB(Vect3d& pb) { _b = pb; }

	/**
	*	@brief Modifica el tercer punto.
	*/
	void setC(Vect3d& pc) { _c = pc; }

	/**
	*	@brief Modifica todos los puntos.
	*/
	void set(Vect3d& va, Vect3d& vb, Vect3d& vc);	
};

