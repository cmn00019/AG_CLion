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

#include "DrawVoxelization.h"

class Voxelization
{
protected:
	class Voxel
	{
	public:
		enum VoxelStatus
		{
			OUTER = 0,
			OCCUPIED = 1,
			INNER = 2
		};

	public:
		AABB							_aabb;
		VoxelStatus						_status;
		std::unordered_set<unsigned>	_triangles;

		Voxel() : _status(OUTER) {}
	};

protected:
	AABB			_aabb;
	vec3			_max, _min;
	//TriangleModel*	_model;
	uvec3			_splits;
	Voxel***		_voxel;
	vec3			_voxelSize;

protected:
	ivec3 getIndices(const vec3& point) const;
	void insertPoint(const vec3& point, unsigned triangleIdx) const;

public:
	//Voxelization(TriangleModel* model, const vec3& voxelSize);
	virtual ~Voxelization();

	AlgGeom::DrawVoxelization* getRenderingObject(bool useColors = true) const;
	void printData() const;
};

