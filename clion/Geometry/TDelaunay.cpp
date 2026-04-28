#include "stdafx.h"
#include "TDelaunay.h"

#include <fstream>
#include <iterator>
#include <iostream>

static std::vector<Point_2> cloudToCGAL(const PointCloud& cloud)
{
    std::vector<Point_2> result;
    std::vector<Point> pts = cloud.getPoints();
    result.reserve(pts.size());
    for (const auto& pt : pts)
    {
        result.emplace_back(pt.getX(), pt.getY());
    }
    return result;
}

TDelaunay::TDelaunay()
{
}

TDelaunay::TDelaunay(const std::string& path)
{
    std::ifstream in(path);
    if (!in.is_open())
    {
        std::cerr << "TDelaunay: No se pudo abrir el fichero " << path << std::endl;
        return;
    }
    std::istream_iterator<Point_2> begin(in);
    std::istream_iterator<Point_2> end;
    _dt.insert(begin, end);
}

TDelaunay::TDelaunay(const PointCloud& cloud)
{
    insert(cloud);
}

void TDelaunay::insert(const Point_2& p)
{
    _dt.insert(p);
}

void TDelaunay::insert(const std::vector<Point_2>& points)
{
    _dt.insert(points.begin(), points.end());
}

void TDelaunay::insert(const PointCloud& cloud)
{
    insert(cloudToCGAL(cloud));
}

size_t TDelaunay::numberOfVertices() const
{
    return _dt.number_of_vertices();
}

std::vector<std::array<Point_2, 3>> TDelaunay::getTriangles() const
{
    std::vector<std::array<Point_2, 3>> triangles;
    for (auto f = _dt.finite_faces_begin(); f != _dt.finite_faces_end(); ++f)
    {
        triangles.push_back({
            f->vertex(0)->point(),
            f->vertex(1)->point(),
            f->vertex(2)->point()
        });
    }
    return triangles;
}

std::vector<Point_2> TDelaunay::getConvexHull() const
{
    std::vector<Point_2> points;
    for (auto v = _dt.finite_vertices_begin(); v != _dt.finite_vertices_end(); ++v)
    {
        points.push_back(v->point());
    }
    std::vector<Point_2> result;
    CGAL::convex_hull_2(points.begin(), points.end(), std::back_inserter(result));
    return result;
}

std::vector<TDelaunay::VoronoiEdge> TDelaunay::getVoronoiEdges() const
{
    std::vector<VoronoiEdge> edges;
    for (auto eit = _dt.edges_begin(); eit != _dt.edges_end(); ++eit)
    {
        CGAL::Object o = _dt.dual(eit);
        if (const Segment_2* s = CGAL::object_cast<Segment_2>(&o))
        {
            edges.push_back({ true, *s, Ray_2() });
        }
        else if (const Ray_2* r = CGAL::object_cast<Ray_2>(&o))
        {
            edges.push_back({ false, Segment_2(), *r });
        }
    }
    return edges;
}

std::vector<Segment_2> TDelaunay::getCroppedVoronoi(double minX, double minY, double maxX, double maxY) const
{
    Iso_rectangle_2 bbox(Point_2(minX, minY), Point_2(maxX, maxY));
    std::vector<Segment_2> result;

    for (auto eit = _dt.edges_begin(); eit != _dt.edges_end(); ++eit)
    {
        CGAL::Object o = _dt.dual(eit);
        if (const Segment_2* s = CGAL::object_cast<Segment_2>(&o))
        {
            CGAL::Object obj = CGAL::intersection(*s, bbox);
            if (const Segment_2* is = CGAL::object_cast<Segment_2>(&obj))
                result.push_back(*is);
        }
        else if (const Ray_2* r = CGAL::object_cast<Ray_2>(&o))
        {
            CGAL::Object obj = CGAL::intersection(*r, bbox);
            if (const Segment_2* is = CGAL::object_cast<Segment_2>(&obj))
                result.push_back(*is);
        }
        else if (const Line_2* l = CGAL::object_cast<Line_2>(&o))
        {
            CGAL::Object obj = CGAL::intersection(*l, bbox);
            if (const Segment_2* is = CGAL::object_cast<Segment_2>(&obj))
                result.push_back(*is);
        }
    }
    return result;
}

Delaunay::Face_handle TDelaunay::locate(const Point_2& p) const
{
    return _dt.locate(p);
}
