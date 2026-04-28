#pragma once

#include "Model3D.h"
#include "TDelaunay.h"

namespace AlgGeom
{
    /**
     * @brief Clase de dibujo para triangulacion de Delaunay.
     * Dibuja los puntos, triangulos (lineas y relleno), envolvente convexa y diagrama de Voronoi.
     */
    class DrawTDelaunay : public Model3D
    {
    protected:
        TDelaunay _delaunay;

    public:
        DrawTDelaunay(TDelaunay& dt, bool drawTriangles = true, bool drawHull = true, bool drawVoronoi = true);
        DrawTDelaunay(const DrawTDelaunay&) = delete;
        ~DrawTDelaunay() override;
    };
}
