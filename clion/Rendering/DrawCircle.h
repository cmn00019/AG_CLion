#pragma once

#include "Circle.h"
#include "Model3D.h"


namespace AlgGeom
{
	class DrawCircle: public Model3D
	{
	protected:
		Circle		_circle;
		uint32_t	_numSubdivisions;

	public:
		DrawCircle(Circle& circle, uint32_t numSubdivisions = 256);
		DrawCircle(const DrawCircle& ddt) = delete;
		~DrawCircle() override {};
	};
}

