#pragma once

/* 
 * File:   Polygon.h
 * Author: lidia
 *
 * Created on 19 de enero de 2021, 10:20
 */

#include <vector>
#include <iostream>
#include "Line.h"
#include "RayLine.h"
#include "SegmentLine.h"
#include "Vect2d.h"
#include "Vertex.h"

    
class Polygon
{
protected:
	std::vector<Vertex> _vertices;

protected:
	/**
	*	@brief Checks if a vertex (not included in the Polygon) creates a segment with the last vertex that collides with any other segment.
	*/
	bool intersectsWithAnySegment(Vertex& vertex);

public:
	/**
	*	@brief Default empty constructor.
	*/
	Polygon();

	/**
	*	@brief Constructor.
	*/
	Polygon(std::vector<Vertex>& vertices);

	/**
	*	@brief Copy constructor.
	*/
	Polygon(const Polygon& Polygon);

	/**
	*	@brief Constructor of a Polygon from a file.
	*/
	Polygon(const std::string& filename);

	/**
	*	@brief Destructor.
	*/
	~Polygon();

	/**
	*	@brief Adds the vertex in the last position (if it doesn't intersect with any other segment).
	*/
	bool add(const Vertex& vertex);

	/**
	*	@brief Adds the point in the last position (if it doesn't intersect with any other segment).
	*/
	bool add(const Point& point);

	/**
	*	@brief Checks if the Polygon is convex.
	*/
	bool convex();

	/**
	*	@brief Returns the edge i.
	*/
	SegmentLine getEdge(int i);

	/**
	*	@brief Returns the number of vertices of the Polygon.
	*/
	size_t getNumVertices() { return _vertices.size(); }

	/**
	*	@brief Returns the vertex at an index.
	*/
	Vertex getVertexAt(int position);

	/**
	*	@brief Checks if a line intersects with the Polygon.
	*	@param intersection If line and Polygon intersect, then this point is the intersection. Otherwise this point is not valid.
	*/
	bool intersects(Line& line, Vect2d& interseccion);

	/**
	*	@brief Checks if a rayline intersects with the Polygon.
	*	@param intersection If rayline and Polygon intersect, then this point is the intersection. Otherwise this point is not valid.
	*/
	bool intersects(RayLine& ray, Vect2d& interseccion);

	/**
	*	@brief Checks if a segment intersects with the Polygon.
	*	@param intersection If Polygon and segment intersect, then this point is the intersection. Otherwise this point is not valid.
	*/
	bool intersects(SegmentLine& segment, Vect2d& interseccion);

	/**
	*	@brief Returns the following vertex based on the specified index.
	*/
	Vertex next(int index);

	/**
	*	@brief Overriding cout call.
	*/
	friend std::ostream& operator<<(std::ostream& os, const Polygon& polygon);

	/**
	*	@brief Returns the vertex previous to the one in the position index.
	*/
	Vertex previous(int index);

	/**
	*	@brief Assignment operator.
	*/
	Polygon& operator=(const Polygon& Polygon);

	/**
	*	@brief Assuming that this is a convex Polygon, this method indicates if the point p is inside the Polygon.
	*/
	bool pointInConvexPolygonGeo(Point& point);

	/**
	*	@brief Saves the coordinates of the Polygon in file with the same format as the constructor.
	*/
	void save(const std::string& filename);

	/**
	*	@brief Modifies the vertex in a position.
	*/
	void set(Vertex& vertex, int pos);
};

