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
 * File:   PontCloud3d.h
 * Author: lidia
 *
 * Created on 26 de enero de 2021, 17:36
 */

#ifndef PONTCLOUD3D_H
#define PONTCLOUD3D_H


#include <string>
#include "AABB.h"
//#include "TriangleMesh.h"
#include "Vect3d.h"

/**
*	@brief This class represents a set of points distributed in the space.
*	@author Lidia Mª Ortega Alvarado.
*/
class PointCloud3d
{
protected:
	std::vector<Vect3d> _points;
	Vect3d _maxPoint, _minPoint;						// AABB.
	Vect3d _maxPointIndex, _minPointIndex;				// Indices of those vertices which have the boundary coordinates of the mesh.

	
protected:
	/**
	*	@brief Updates the new maximum and minimum points taking into account a new point.
	*/
	void updateMaxMin(int index);

public:
	/**
	*	@brief Default constructor.
	*/
	PointCloud3d();

	/**
	*	@brief Loads the point cloud from a file.
	*/
	PointCloud3d(const std::string& filename);

        /**
	*	@brief Construct the point cloud from a vector of points
	*/
	PointCloud3d(std::vector<Vect3d>& pointCloud);
        
	/**
	*	@brief Constructor.
	*/
	PointCloud3d(int size, float max_x, float max_y, float max_z);

	/**
	*	@brief Alternative constructor with an uniform sphere distribution.
	*/
	PointCloud3d(int size, float radius);

	/**
	*	@brief Copy constructor.
	*/
	PointCloud3d(const PointCloud3d& pointCloud);

	/**
	*	@brief Destructor.
	*/
	virtual ~PointCloud3d();

	/**
	*	@brief Adds a new point to the cloud.
	*/
	void addPoint(Vect3d& p);

	/**
	*	@brief Removes all the points.
	*/
	void clear() { _points.clear(); }

	/**
	*	@brief Deletes the data saved from the convex hull step by step process.
	*/
	//void forgetConvexHullData() { delete _convexHullData; _convexHullData = nullptr; }

	/**
	*	@brief Returns the bounding box that delimites the point cloud space.
	*/
	AABB getAABB();

	/**
	*    @brief get the index of the most distanced points in the cloud.
	*/
	void getMostDistanced(int& a, int& b);

	/**
	*	@brief Generates a complete convex hull.
	*/
	//TriangleMesh* getConvexHull();

	/**
	*	@brief Generates the next triangle from the current convex hull.
	*/
	//TriangleMesh* getConvexHullNextTriangle();

	/**
	*	@brief Returns a certain point.
	*/
	Vect3d getPoint(int pos);

	/**
	*	@brief Returns all the cloud points.
	*/
	std::vector<Vect3d> getPoints() { return _points; }

	/**
	*	@brief Assigment operator.
	*/
	virtual PointCloud3d& operator=(const PointCloud3d& pointCloud);

	/**
	*	@brief Saves the cloud points in a file.
	*/
	void save(const std::string& filename);

	/**
	*	@brief Returns the number of points that this cloud contains.
	*/
	size_t size() { return _points.size(); }
};



#endif /* PONTCLOUD3D_H */

