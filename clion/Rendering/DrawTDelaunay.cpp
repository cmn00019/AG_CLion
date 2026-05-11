#include "stdafx.h"
#include "DrawTDelaunay.h"

#include <limits>
#include <cmath>

AlgGeom::DrawTDelaunay::DrawTDelaunay(TDelaunay& dt, bool drawTriangles, bool drawHull, bool drawVoronoi)
    : Model3D(), _delaunay(dt)
{
    // ============================================================
    // Extraer vertices unicos de la triangulacion
    // ============================================================
    std::vector<Point_2> uniquePoints;
    for (auto v = dt.getDelaunay().finite_vertices_begin(); v != dt.getDelaunay().finite_vertices_end(); ++v)
    {
        uniquePoints.push_back(v->point());
    }

    // Funcion lambda para obtener indice de un punto
    auto getIndex = [&](const Point_2& p) -> GLuint {
        for (size_t i = 0; i < uniquePoints.size(); ++i)
        {
            if (uniquePoints[i] == p)
                return (GLuint)i;
        }
        return 0;
    };

    // ============================================================
    // Componente de puntos (siempre visible, tamaño grande)
    // ============================================================
    Component* pointsComp = new Component;
    for (const auto& p : uniquePoints)
    {
        pointsComp->_vertices.push_back(VAO::Vertex{ vec3((float)p.x(), (float)p.y(), 0.0f), vec3(0.0f, 0.0f, 1.0f) });
    }
    for (size_t i = 0; i < uniquePoints.size(); ++i)
    {
        pointsComp->_indices[VAO::IBO_POINT].push_back((GLuint)i);
    }
    pointsComp->_material._pointColor = vec3(0.0f, 0.0f, 1.0f);          // azul
    pointsComp->_material._useUniformColor = true;
    pointsComp->_pointSize = 12.0f;
    pointsComp->completeTopology();
    _pointsCompIdx = (int)this->_components.size();
    this->_components.push_back(std::unique_ptr<Component>(pointsComp));
    this->buildVao(pointsComp);

    // ============================================================
    // Componente de triangulos y aristas Delaunay
    // ============================================================
    Component* delaunayComp = new Component;
    for (const auto& p : uniquePoints)
    {
        delaunayComp->_vertices.push_back(VAO::Vertex{ vec3((float)p.x(), (float)p.y(), 0.0f), vec3(0.0f, 0.0f, 1.0f) });
    }

    auto triangles = dt.getTriangles();
    for (const auto& tri : triangles)
    {
        GLuint i0 = getIndex(tri[0]);
        GLuint i1 = getIndex(tri[1]);
        GLuint i2 = getIndex(tri[2]);

        delaunayComp->_indices[VAO::IBO_TRIANGLE].insert(delaunayComp->_indices[VAO::IBO_TRIANGLE].end(),
            { i0, i1, i2, RESTART_PRIMITIVE_INDEX });
        delaunayComp->_indices[VAO::IBO_LINE].insert(delaunayComp->_indices[VAO::IBO_LINE].end(),
            { i0, i1, i1, i2, i2, i0 });
    }

    delaunayComp->_material._lineColor = vec3(0.0f, 0.0f, 0.0f);            // negro (Delaunay)
    delaunayComp->_material._kdColor = vec4(0.5f, 0.8f, 1.0f, 0.3f);       // cyan transparente
    delaunayComp->_material._useUniformColor = true;
    delaunayComp->_lineWidth = 1.0f;

    delaunayComp->completeTopology();
    _delaunayCompIdx = (int)this->_components.size();
    this->_components.push_back(std::unique_ptr<Component>(delaunayComp));
    this->buildVao(delaunayComp);
    this->calculateAABB();

    // Calcular bounding box para recortar Voronoi
    float minX = (std::numeric_limits<float>::max)();
    float minY = (std::numeric_limits<float>::max)();
    float maxX = -(std::numeric_limits<float>::max)();
    float maxY = -(std::numeric_limits<float>::max)();

    for (const auto& p : uniquePoints)
    {
        minX = std::min(minX, (float)p.x());
        minY = std::min(minY, (float)p.y());
        maxX = std::max(maxX, (float)p.x());
        maxY = std::max(maxY, (float)p.y());
    }

    float margin = std::max(maxX - minX, maxY - minY) * 0.5f + 1.0f;
    if (margin < 1.0f || std::isnan(margin) || std::isinf(margin))
        margin = 5.0f;

    // ============================================================
    // Optativo I: Envolvente convexa (lineas rojas)
    // ============================================================
    if (drawHull)
    {
        auto hull = dt.getConvexHull();
        if (hull.size() >= 2)
        {
            Component* hullComp = new Component;
            for (const auto& p : hull)
            {
                hullComp->_vertices.push_back(VAO::Vertex{ vec3((float)p.x(), (float)p.y(), 0.0f), vec3(0.0f, 0.0f, 1.0f) });
            }
            for (size_t i = 0; i < hull.size(); ++i)
            {
                GLuint a = (GLuint)i;
                GLuint b = (GLuint)((i + 1) % hull.size());
                hullComp->_indices[VAO::IBO_LINE].insert(hullComp->_indices[VAO::IBO_LINE].end(), { a, b });
            }
            hullComp->_material._lineColor = vec3(1.0f, 0.0f, 0.0f); // rojo
            hullComp->_lineWidth = 2.0f;
            hullComp->completeTopology();
            _hullCompIdx = (int)this->_components.size();
            this->_components.push_back(std::unique_ptr<Component>(hullComp));
            this->buildVao(hullComp);
        }
    }

    // ============================================================
    // Optativo II: Diagrama de Voronoi recortado (lineas verdes)
    // ============================================================
    if (drawVoronoi)
    {
        auto vEdges = dt.getCroppedVoronoi(minX - margin, minY - margin, maxX + margin, maxY + margin);
        if (!vEdges.empty())
        {
            Component* vorComp = new Component;
            for (const auto& seg : vEdges)
            {
                GLuint base = (GLuint)vorComp->_vertices.size();
                vorComp->_vertices.push_back(VAO::Vertex{
                    vec3((float)seg.source().x(), (float)seg.source().y(), 0.0f), vec3(0.0f, 0.0f, 1.0f) });
                vorComp->_vertices.push_back(VAO::Vertex{
                    vec3((float)seg.target().x(), (float)seg.target().y(), 0.0f), vec3(0.0f, 0.0f, 1.0f) });
                vorComp->_indices[VAO::IBO_LINE].insert(vorComp->_indices[VAO::IBO_LINE].end(), { base, base + 1 });
            }
            vorComp->_material._lineColor = vec3(0.0f, 1.0f, 0.0f); // verde
            vorComp->_lineWidth = 1.5f;
            vorComp->completeTopology();
            _voronoiCompIdx = (int)this->_components.size();
            this->_components.push_back(std::unique_ptr<Component>(vorComp));
            this->buildVao(vorComp);
        }
    }
}

void AlgGeom::DrawTDelaunay::setDelaunayVisible(bool visible)
{
    if (_delaunayCompIdx >= 0 && _delaunayCompIdx < (int)_components.size())
        _components[_delaunayCompIdx]->_enabled = visible;
}

void AlgGeom::DrawTDelaunay::setHullVisible(bool visible)
{
    if (_hullCompIdx >= 0 && _hullCompIdx < (int)_components.size())
        _components[_hullCompIdx]->_enabled = visible;
}

void AlgGeom::DrawTDelaunay::setVoronoiVisible(bool visible)
{
    if (_voronoiCompIdx >= 0 && _voronoiCompIdx < (int)_components.size())
        _components[_voronoiCompIdx]->_enabled = visible;
}

AlgGeom::DrawTDelaunay::~DrawTDelaunay()
{
}
