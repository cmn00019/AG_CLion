#include "stdafx.h"
#include "PointCloud.h"
#include "RandomUtilities.h" // Incluimos la utilidad de aleatorios
#include <fstream>
#include <limits>
#include <algorithm> // Para std::min y std::max

PointCloud::PointCloud()
{
}

// Constructor aleatorio usando RandomUtilities
PointCloud::PointCloud(int size, float max_x, float max_y)
{
    _points.reserve(size);

    for (int i = 0; i < size; i++)
    {
        float x = RandomUtilities::getUniformRandom(0.0f, max_x);
        float y = RandomUtilities::getUniformRandom(0.0f, max_y);

        addPoint(Point(x, y));
    }
}

PointCloud::PointCloud(const std::string& filename)
{
    std::ifstream file(filename);
    if (file.is_open())
    {
        double x, y;
        while (file >> x >> y)
        {
            addPoint(Point(x, y));
        }
        file.close();
    }
}

PointCloud::~PointCloud()
{
}

void PointCloud::addPoint(const Point& p)
{
    _points.push_back(p);
}

Point PointCloud::centralPoint()
{
    if (_points.empty()) return Point();

    double minMaxDist = std::numeric_limits<double>::max();
    int centralIndex = -1;

    for (size_t i = 0; i < _points.size(); ++i)
    {
        double currentMaxDist = 0.0;

        for (size_t j = 0; j < _points.size(); ++j)
        {
            if (i != j) {
                double d = _points[i].distance(_points[j]);

                if (d > currentMaxDist) {
                    currentMaxDist = d;
                }
            }
        }

        if (currentMaxDist < minMaxDist) {
            minMaxDist = currentMaxDist;
            centralIndex = static_cast<int>(i);
        }
    }

    if (centralIndex != -1) {
        return _points[centralIndex];
    }

    return _points[0];
}

void PointCloud::deletePoint(int index)
{
    if (index >= 0 && index < _points.size())
    {
       _points.erase(_points.begin() + index);
    }
}

void PointCloud::getEdges(Point& minPoint_x, Point& minPoint_y, Point& maxPoint_x, Point& maxPoint_y)
{
    if (_points.empty()) return;

    // Inicializamos todos con el primer punto
    minPoint_x = maxPoint_x = minPoint_y = maxPoint_y = _points[0];

    for (size_t i = 1; i < _points.size(); ++i)
    {
        Point current = _points[i];

        // Buscamos extremos en X
        if (current.getX() < minPoint_x.getX()) minPoint_x = current;
        if (current.getX() > maxPoint_x.getX()) maxPoint_x = current;

        // Buscamos extremos en Y
        if (current.getY() < minPoint_y.getY()) minPoint_y = current;
        if (current.getY() > maxPoint_y.getY()) maxPoint_y = current;
    }
}

Point PointCloud::getPoint(int position)
{
    if ((position >= 0) && (position < _points.size()))
    {
       return _points[position];
    }

    return Point();
}

PointCloud & PointCloud::operator=(const PointCloud& pointCloud)
{
    if (this != &pointCloud)
    {
       this->_points = pointCloud._points;
    }

    return *this;
}

void PointCloud::save(const std::string& filename)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        for (const auto& p : _points)
        {
            Point pt = p;
            file << pt.getX() << " " << pt.getY() << "\n";
        }
        file.close();
    }
}