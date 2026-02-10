#include "stdafx.h"

namespace BasicGeometry
{
	bool equal(double a, double b);
	double determinant2x2(double a, double b, double c, double d);
	double determinant3x3(double a, double b, double c, double d, double e, double f, double g, double h, double i);
	double min3(double a, double b, double c);
	double max3(double a, double b, double c);
	double min2(double a, double b);
	double max2(double a, double b);   
};

inline bool BasicGeometry::equal(double a, double b) 
{
	return (abs(a - b) < glm::epsilon<double>());
}

inline double BasicGeometry::determinant2x2(double a, double b, double c, double d)
{
	return (a * c - b * d);
}

inline double BasicGeometry::determinant3x3(double a, double b, double c, double d, double e, double f, double g, double h, double i)
{
	return (a * e * i + g * b * f + c * d * h - c * e * g - i * d * b - a * h * f);
}

inline double BasicGeometry::min3(double a, double b, double c)
{
	return (a < b ? (a < c ? a : c) : (b < c ? b : c));
}

inline double BasicGeometry::max3(double a, double b, double c)
{
	return (a > b ? (a > c ? a : c) : (b > c ? b : c));
}

inline double BasicGeometry::min2(double a, double b)
{
	return (a < b ? a : b);
}

inline double BasicGeometry::max2(double a, double b)
{
	return (a > b ? a : b);
}