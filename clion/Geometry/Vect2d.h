#pragma once

/* 
 * File:   Vect2d.h
 * Author: lidia
 *
 * Created on 5 de enero de 2021, 14:01
 */

#include "Point.h"

class Vect2d : public Point  
{
public:      
    Vect2d () : Point () { }
    Vect2d (double x, double y): Point (x, y) { } 
   
	/**
	*	@return X coordinate.
	*/
    double getX() {return _x; }

	/**
	*	@return Y coordinate.
	*/
    double getY() {return _y; }
    
    /**
	*	@brief Copy constructor.
	*/
	Vect2d(const Vect2d& vector);

	/**
	*	@brief Copy constructor.
	*/
	Vect2d(const Point& point);

	/**
	*	@brief Destructor.
	*/
    ~Vect2d() override;

	/**
	*	@brief Returns a . b (scalar product).
	*/
	double dot(Vect2d& b);

	/**
	*	@brief Assignment operator.
	*/
	virtual Vect2d& operator=(const Vect2d& vector);

	/**
	*	@brief Returns this + b (sum of vectors).
	*/
	Vect2d operator+(const Vect2d& b);

	/**
	*	@brief Returns a - b (substraction of vectors).
	*/
	Vect2d operator-(const Vect2d& b);

	/**
	*	@brief Returns t . a (vector product by a scalar).
	*/
	Vect2d scalarMult(double t);
};

