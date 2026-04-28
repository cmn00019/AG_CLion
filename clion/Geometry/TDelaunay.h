#pragma once

#include "PointCloud.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/convex_hull_2.h>
#include <vector>
#include <array>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;
typedef K::Segment_2 Segment_2;
typedef K::Ray_2 Ray_2;
typedef K::Line_2 Line_2;
typedef K::Iso_rectangle_2 Iso_rectangle_2;
typedef CGAL::Delaunay_triangulation_2<K> Delaunay;

/**
 * @brief Clase que encapsula la funcionalidad de triangulacion de Delaunay con CGAL.
 */
class TDelaunay
{
protected:
    Delaunay _dt;

public:
    TDelaunay();
    TDelaunay(const std::string& path);
    TDelaunay(const PointCloud& cloud);

    void insert(const Point_2& p);
    void insert(const std::vector<Point_2>& points);
    void insert(const PointCloud& cloud);

    size_t numberOfVertices() const;

    /** @brief Devuelve los triangulos finitos de la triangulacion. */
    std::vector<std::array<Point_2, 3>> getTriangles() const;

    /** @brief Optativo I: Devuelve la envolvente convexa de los vertices. */
    std::vector<Point_2> getConvexHull() const;

    /** @brief Estructura para aristas de Voronoi (segmento o rayo). */
    struct VoronoiEdge {
        bool isSegment;
        Segment_2 segment;
        Ray_2 ray;
    };

    /** @brief Optativo II: Devuelve las aristas del diagrama de Voronoi (sin recortar). */
    std::vector<VoronoiEdge> getVoronoiEdges() const;

    /** @brief Optativo II: Devuelve las aristas de Voronoi recortadas a un bounding box. */
    std::vector<Segment_2> getCroppedVoronoi(double minX, double minY, double maxX, double maxY) const;

    /** @brief Localiza un punto en la triangulacion. */
    Delaunay::Face_handle locate(const Point_2& p) const;

    const Delaunay& getDelaunay() const { return _dt; }
};
