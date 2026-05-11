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
        int _pointsCompIdx = -1;
        int _delaunayCompIdx = -1;
        int _hullCompIdx = -1;
        int _voronoiCompIdx = -1;

    public:
        DrawTDelaunay(TDelaunay& dt, bool drawTriangles = true, bool drawHull = true, bool drawVoronoi = true);
        DrawTDelaunay(const DrawTDelaunay&) = delete;
        ~DrawTDelaunay() override;

        void setDelaunayVisible(bool visible);
        void setHullVisible(bool visible);
        void setVoronoiVisible(bool visible);
    };
}
