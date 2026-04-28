#include "stdafx.h"
#include "Window.h"

// ========================================================================
// Ejemplo terrain.cpp (ya funcionando anteriormente en consola)
// Se conserva aqui como referencia del ejercicio explicado en clase.
// ========================================================================
/*
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Projection_traits_xy_3.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <fstream>
#include <iostream>
#include <iterator>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Projection_traits_xy_3<K>  Gt;
typedef CGAL::Delaunay_triangulation_2<Gt> Delaunay;
typedef K::Point_3   Point;

void terrainExample()
{
    std::ifstream in("terrain.cin");
    if (!in.is_open()) {
        std::cerr << "No se ha podido encontrar terrain.cin!" << std::endl;
        return;
    }
    std::istream_iterator<Point> begin(in);
    std::istream_iterator<Point> end;
    Delaunay dt(begin, end);
    std::cout << "Vertices terrain: " << dt.number_of_vertices() << std::endl;
}
*/

int main()
{
    try
    {
        AlgGeom::Window* window = AlgGeom::Window::getInstance();
        window->init("Algoritmos Geometricos - Delaunay", 1280, 720);
        window->loop();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
