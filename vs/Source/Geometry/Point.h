#pragma once

#include "BasicGeometry.h"

/**
*	@brief This class represents a 2D structure. This will be also the skeleton for other 2D structures such as Vertex or Vector.
*	@author Lidia 
*/
class Point
{
protected:
	const static int DEFAULT_VALUE = INT_MAX;					// Value of X and Y coordinates for an incomplete Point.

public:
	enum PointClassification 
	{
		LEFT, RIGHT, FORWARD, BACKWARD, BETWEEN, ORIGIN, DEST
	};

protected:
	double _x, _y;

public:
	/**
	*	@brief Default constructor. Coordinates are initialized with an invalid value.
	*/
	Point();

	/**
	*	@brief Constructor. If polar, x is an angle (radians) and y the vector module.
	*/
	Point(double x, double y, bool polar = false);

	/**
	*	@brief Copy constructor.
	*/
	Point(const Point& point);

	/**
	*	@brief Destructor.
	*/
	virtual ~Point();

	/**
	*	@brief Checks the position of the point respect to other two points (a, b).
	*/
	bool backward(Point& a, Point& b) { return this->classify(a, b) == PointClassification::BACKWARD; }

	/**
	*	@brief Determines the relative position with respect to other two (which can form a segment).
	*/
	PointClassification classify(Point& p0, Point& p1);

	/**
	*	@brief Checks if this point lies on the same line described by a and b.
	*/
	bool colinear(Point& a, Point& b);

	/**
	*	@brief Distance between this point and another one.
	*/
	double distance(Point& p);

	/**
	*	@brief Checks if the coordinates of this point are different from the coordinates of a point p.
	*/
	bool distinct(Point& p) { return BasicGeometry::equal(_x, p._x) or BasicGeometry::equal(_y, p._y); }

	/**
	*	@brief Checks if the coordinates of this point are equal from the coordinates of a point p.
	*/
	bool equal(Point& p) { return BasicGeometry::equal(_x, p._x) and BasicGeometry::equal(_y, p._y); }

	 /**
	 *	@brief Checks the position of the point respect to other two points (a, b).
	 */
	bool forward(Point& a, Point& b) { return classify(a, b) == PointClassification::FORWARD; }

	/**
	*	@return X coordinate.
	*/
	double getX() { return _x; }

	/**
	*	@return Y coordinate.
	*/
	double getY() { return _y; }

	/**
	*	@brief Angle of this point interpreted as a polar coordinate (radians).
	*/
	double getAlpha();

	/**
	*	@brief Module of _a 2D structure. This method is useful for child classes.
	*/
	double getModule();

	/**
	*	@brief Checks the position of the point respect to other two points (a, b).
	*/
	bool isBetween(Point& a, Point& b) { return classify(a, b) == PointClassification::BETWEEN; }

	/**
	*	@brief Checks the value of the coordinates. If the values are the DEFAULT, then the point is not valid.
	*/
	bool isValid() { return (_x != DEFAULT_VALUE) && (_y != DEFAULT_VALUE); }

	/**
	*	@brief Checks the position of the point respect to other two points (a, b).
	*/
	bool left(Point& a, Point& b) { return classify(a, b) == PointClassification::LEFT; }

	/**
	*	@brief Checks the position of the point respect to other two points (a, b).
	*/
	bool leftAbove(Point& a, Point& b);

	/**
	*	@brief Assignment operator (override).
	*/
	Point& operator=(const Point& point);

	/**
	*	@brief Overriding cout call.
	*/
	friend std::ostream& operator<<(std::ostream& os, const Point& point);

	/**
	*	@brief Checks the position of the point respect to other two points (a, b).
	*/
	bool rightAbove(Point& a, Point& b);

	/**
	*	@brief Checks the position of the point respect to other two points (a, b).
	*/
	bool right(Point& a, Point& b) { return classify(a, b) == PointClassification::RIGHT; }

	/**
	*	@brief Modifies the coordinate values.
	*/
	void set(double x, double y) { this->_x = x; this->_y = y; }

	/**
	*	@brief Modifies the X coordinate.
	*/
	void setX(double x) { _x = x; }

	/**
	*	@brief Modifies the Y coordinate.
	*/
	void setY(double y) { _y = y; }

	/**
	*	@brief Returns the slope between this point and p.
	*/
	double slope(Point& p);

	/**
	*	@brief Calculates the double area of the triangle formed by (this, a, b).
	*/
	double triangleArea2(Point& a, Point& b);
};

