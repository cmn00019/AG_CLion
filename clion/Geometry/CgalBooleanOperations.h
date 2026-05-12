#pragma once

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/corefinement.h>
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Edge_count_ratio_stop_predicate.h>
#include <CGAL/boost/graph/helpers.h>
#include <CGAL/Polygon_mesh_processing/orientation.h>
#include <CGAL/boost/graph/IO/OBJ.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>

#include "TriangleModel.h"
#include "Triangle3d.h"
#include "Vect3d.h"

#include "stdafx.h"

namespace PMP = CGAL::Polygon_mesh_processing;
namespace SMS = CGAL::Surface_mesh_simplification;

class CgalBooleanOperations
{
public:
    typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
    typedef Kernel::Point_3 Point_3;
    typedef CGAL::Surface_mesh<Point_3> SurfaceMesh;

private:
    SurfaceMesh _meshA;
    SurfaceMesh _meshB;
    SurfaceMesh _meshA_orig;
    SurfaceMesh _meshB_orig;
    SurfaceMesh _result;
    bool _hasA = false;
    bool _hasB = false;
    bool _hasResult = false;

    static SurfaceMesh triangleModelToSurfaceMesh(TriangleModel& model);
    static void surfaceMeshToTriangleModel(const SurfaceMesh& mesh, std::vector<Triangle3d>& outTriangles);
    static bool loadOFFToSurfaceMesh(const std::string& path, SurfaceMesh& mesh);
    static bool loadOBJToSurfaceMesh(const std::string& path, SurfaceMesh& mesh);
    static Point_3 transformPoint(const Point_3& p, const mat4& m);

public:
    CgalBooleanOperations() = default;
    virtual ~CgalBooleanOperations() = default;

    // Carga
    bool loadModelA(const std::string& path);
    bool loadModelB(const std::string& path);
    bool setModelA(TriangleModel& model);
    bool setModelB(TriangleModel& model);

    // Verificación
    static bool isValidForBoolean(const SurfaceMesh& mesh);
    bool isValidA() const { return _hasA && isValidForBoolean(_meshA); }
    bool isValidB() const { return _hasB && isValidForBoolean(_meshB); }

    // Aplicar transformaciones visuales antes de ejecutar operaciones
    void applyModelMatrixToMeshes(const mat4& matA, const mat4& matB);

    // Operaciones booleanas (usa internamente corefine_and_compute_boolean_operations)
    bool computeUnion();
    bool computeIntersection();
    bool computeDifferenceAB(); // A - B
    bool computeDifferenceBA(); // B - A

    // Simplificación del resultado
    bool simplifyResult(double ratio); // ratio en [0,1], 1.0 = sin cambios

    // Exportación
    TriangleModel* exportResultToTriangleModel() const;
    bool saveResultToOBJ(const std::string& path) const;
    bool saveResultToOFF(const std::string& path) const;

    // Info
    void printInfoA() const;
    void printInfoB() const;
    void printInfoResult() const;
    size_t getAVertices() const { return _hasA ? _meshA.number_of_vertices() : 0; }
    size_t getAFaces() const { return _hasA ? _meshA.number_of_faces() : 0; }
    size_t getBVertices() const { return _hasB ? _meshB.number_of_vertices() : 0; }
    size_t getBFaces() const { return _hasB ? _meshB.number_of_faces() : 0; }
    size_t getResultVertices() const;
    size_t getResultFaces() const;

    bool hasA() const { return _hasA; }
    bool hasB() const { return _hasB; }
    bool hasResult() const { return _hasResult; }
    void clearResult() { _hasResult = false; _result.clear(); }
};
