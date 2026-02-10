#include "stdafx.h"

#include "Voxelization.h"


// Public methods

//Voxelization::Voxelization(TriangleModel* model, const vec3& voxelSize): _model(model), _voxelSize(voxelSize)
//{
//	AABB aabb = model->getAABB();
//	vec3 aabbSize = aabb.size();
//	_splits = glm::ceil(aabbSize / voxelSize);
//	_aabb = AABB(aabb.center() - vec3(_splits) / vec3(2.0f) * _voxelSize, aabb.center() + vec3(_splits) / vec3(2.0f) * _voxelSize);
//
//	_voxel = new Voxel**[_splits.x];
//	for (unsigned x = 0; x < _splits.x; ++x)
//	{
//		_voxel[x] = new Voxel*[_splits.y];
//		for (unsigned y = 0; y < _splits.y; ++y)
//		{
//			_voxel[x][y] = new Voxel[_splits.z];
//			for (unsigned z = 0; z < _splits.z; ++z)
//			{
//				_voxel[x][y][z]._aabb = AABB(_aabb.min() + vec3(x, y, z) * _voxelSize, _aabb.min() + vec3(x + 1, y + 1, z + 1) * _voxelSize);
//			}
//		}
//	}
//}

Voxelization::~Voxelization()
{
	for (unsigned x = 0; x < _splits.x; ++x)
	{
		for (unsigned y = 0; y < _splits.y; ++y)
		{
			delete[] _voxel[x][y];
		}
		delete _voxel[x];
	}
	delete[] _voxel;
}

AlgGeom::DrawVoxelization* Voxelization::getRenderingObject(bool useColors) const
{
	AlgGeom::DrawVoxelization* voxelization = nullptr;

	if (useColors)
	{
		unsigned linearIndex = 0, numVoxels = _splits.x * _splits.y * _splits.z;
		vec3* positions = new vec3[numVoxels];
		vec3* colors = new vec3[numVoxels];
		vec3 color[] = { vec3(.0f), vec3(.5f), vec3(1.0f) };

		for (unsigned x = 0; x < _splits.x; ++x)
		{
			for (unsigned y = 0; y < _splits.y; ++y)
			{
				for (unsigned z = 0; z < _splits.z; ++z)
				{
					linearIndex = z + y * _splits.z + x * _splits.z * _splits.y;
					positions[linearIndex] = _voxel[x][y][z]._aabb.center();
					colors[linearIndex] = color[_voxel[x][y][z]._status];
				}
			}
		}

		voxelization = new AlgGeom::DrawVoxelization(positions, numVoxels, _voxelSize, colors);
		delete[] positions;
		delete[] colors;
	}
	else
	{
		std::vector<vec3> positions;

		for (unsigned x = 0; x < _splits.x; ++x)
		{
			for (unsigned y = 0; y < _splits.y; ++y)
			{
				for (unsigned z = 0; z < _splits.z; ++z)
				{
					if (_voxel[x][y][z]._status == Voxel::INNER)
						positions.push_back(_voxel[x][y][z]._aabb.center());
				}
			}
		}

		voxelization = new AlgGeom::DrawVoxelization(positions.data(), positions.size(), _voxelSize, nullptr);
	}

	return voxelization;
}

void Voxelization::printData() const
{
	unsigned numOccupiedVoxels = 0;
	unsigned numInnerVoxels = 0;
	unsigned numOuterVoxels = 0;

	for (unsigned x = 0; x < _splits.x; ++x)
	{
		for (unsigned y = 0; y < _splits.y; ++y)
		{
			for (unsigned z = 0; z < _splits.z; ++z)
			{
				numOccupiedVoxels += static_cast<unsigned>(_voxel[x][y][z]._status == Voxel::OCCUPIED);
				numInnerVoxels += static_cast<unsigned>(_voxel[x][y][z]._status == Voxel::INNER);
				numOuterVoxels += static_cast<unsigned>(_voxel[x][y][z]._status == Voxel::OUTER);
			}
		}
	}

	std::cout << "Number of filled voxels: " << numOccupiedVoxels << std::endl;
	std::cout << "Number of inner voxels: " << numInnerVoxels << std::endl;
	std::cout << "Number of outer voxels: " << numOuterVoxels << std::endl;
}

// Protected methods

ivec3 Voxelization::getIndices(const vec3& point) const
{
	return ivec3((point - _aabb.min()) / _voxelSize);
}

void Voxelization::insertPoint(const vec3& point, unsigned triangleIdx) const
{
	ivec3 indices = this->getIndices(point);
	_voxel[indices.x][indices.y][indices.z]._triangles.insert(triangleIdx);
	_voxel[indices.x][indices.y][indices.z]._status = Voxel::OCCUPIED;
}
