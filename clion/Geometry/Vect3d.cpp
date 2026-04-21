#include "stdafx.h"
#include "Vect3d.h"

#include "BasicGeometry.h"
#include "Triangle3d.h"


Vect3d::Vect3d()
{
	this->setVert(FLT_MAX, FLT_MAX, FLT_MAX);
}

Vect3d::Vect3d(double x, double y, double z)
{
	this->_value[X] = x;
	this->_value[Y] = y;
	this->_value[Z] = z;
}

Vect3d::Vect3d(const Vect3d & vector)
{
	_value[X] = vector._value[X];
	_value[Y] = vector._value[Y];
	_value[Z] = vector._value[Z];
}

Vect3d::~Vect3d()
{
}

Vect3d Vect3d::add(const Vect3d & b) const
{
	return Vect3d(getX() + b.getX(), getY() + b.getY(), getZ() + b.getZ());
}

bool Vect3d::collinear(const Vect3d & a, const Vect3d & b) const
{
	Triangle3d tr (a, b, *this);

	return BasicGeometry::equal(tr.area(), glm::epsilon<double>());
}

double Vect3d::distance(const Vect3d & p) const
{
    Vect3d diff = this->sub(p);
    return diff.module();
}

double Vect3d::dot(const Vect3d & v) const
{
	return (getX() * v.getX() + getY() * v.getY() + getZ() * v.getZ());
}

std::vector<double> Vect3d::getVert() const
{
	return std::vector<double> {getX(), getY(), getZ()};
}

double Vect3d::getX() const
{
	return _value[X];
}

double Vect3d::getY() const
{
	return _value[Y];
}

double Vect3d::getZ() const
{
	return _value[Z];
}

double Vect3d::module() const
{
	return sqrt(getX() * getX() + getY() * getY() + getZ() * getZ());
}

Vect3d& Vect3d::operator=(const Vect3d & vector)
{
	this->_value[X] = vector._value[X];
	this->_value[Y] = vector._value[Y];
	this->_value[Z] = vector._value[Z];

	return *this;
}

bool Vect3d::operator==(const Vect3d & vector) const
{
	return BasicGeometry::equal(getX(), vector._value[X]) && BasicGeometry::equal(getY(), vector._value[Y]) && BasicGeometry::equal(getZ(), vector._value[Z]);
}

bool Vect3d::operator!=(const Vect3d & vector) const
{
	return !(this->operator==(vector));
}

std::ostream& operator<<(std::ostream& os, const Vect3d& vec)
{
	os << "Coordenada X: " << std::to_string(vec.X) << ", coordenada Y: " << std::to_string(vec.Y) << ", coordenada Z: " << std::to_string(vec.Z);
	return os;
}

Vect3d Vect3d::scalarMul(double value) const
{
	return Vect3d(getX() * value, getY() * value, getZ() * value);
}

void Vect3d::setX(double x)
{
	this->_value[X] = x;
}

void Vect3d::setY(double y)
{
	this->_value[Y] = y;
}

void Vect3d::setZ(double z)
{
	this->_value[Z] = z;
}

void Vect3d::setVert(double x, double y, double z)
{
	this->_value[X] = x;
	this->_value[Y] = y;
	this->_value[Z] = z;
}

Vect3d Vect3d::sub(const Vect3d & b) const
{
	return Vect3d(getX() - b.getX(), getY() - b.getY(), getZ() - b.getZ());
}

Vect3d Vect3d::xProduct(const Vect3d & b) const
{
	return Vect3d(getY() * b.getZ() - getZ() * b.getY(), getZ() * b.getX() - getX() * b.getZ(), getX() * b.getY() - getY() * b.getX());
}

void Vect3d::getPlane(const Vect3d& v, Vect3d& n, float& d) const
{
    // plano equidistante
    // n = v - this (normal)
    n = v.sub(*this);
    // punto medio
    double mx = (getX() + v.getX()) / 2.0;
    double my = (getY() + v.getY()) / 2.0;
    double mz = (getZ() + v.getZ()) / 2.0;
    // d = -(n . puntomedio)
    d = (float)(-(n.getX() * mx + n.getY() * my + n.getZ() * mz));
}