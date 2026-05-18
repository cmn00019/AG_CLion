#include "stdafx.h"
#include "CgalBooleanOperations.h"

#include <fstream>
#include <iostream>
#include <chrono>
#include <array>
#include <optional>
#include <CGAL/Polygon_mesh_processing/repair.h>

static void repairAndOrientMesh(CgalBooleanOperations::SurfaceMesh& mesh)
{
    PMP::remove_isolated_vertices(mesh);
    if (CGAL::is_closed(mesh) && CGAL::is_triangle_mesh(mesh))
    {
        try
        {
            if (!PMP::is_outward_oriented(mesh))
            {
                std::cout << "[CgalBoolean] Orientacion inward detectada. Invirtiendo caras..." << std::endl;
                PMP::reverse_face_orientations(mesh);
            }
        }
        catch (...)
        {
            // Si la orientacion no puede determinarse, continuamos sin cambios
        }
    }
}

static bool areMeshesIdentical(const CgalBooleanOperations::SurfaceMesh& a, const CgalBooleanOperations::SurfaceMesh& b)
{
    if (a.number_of_vertices() != b.number_of_vertices()) return false;
    if (a.number_of_faces() != b.number_of_faces()) return false;

    CGAL::Bbox_3 bbox_a, bbox_b;
    bool first = true;
    for (auto v : a.vertices()) {
        auto p = a.point(v);
        if (first) { bbox_a = p.bbox(); first = false; }
        else bbox_a = bbox_a + p.bbox();
    }
    first = true;
    for (auto v : b.vertices()) {
        auto p = b.point(v);
        if (first) { bbox_b = p.bbox(); first = false; }
        else bbox_b = bbox_b + p.bbox();
    }

    const double eps = 1e-9;
    auto eq = [eps](double x, double y) { return std::abs(x - y) < eps; };

    return eq(bbox_a.xmin(), bbox_b.xmin()) && eq(bbox_a.xmax(), bbox_b.xmax()) &&
           eq(bbox_a.ymin(), bbox_b.ymin()) && eq(bbox_a.ymax(), bbox_b.ymax()) &&
           eq(bbox_a.zmin(), bbox_b.zmin()) && eq(bbox_a.zmax(), bbox_b.zmax());
}

CgalBooleanOperations::SurfaceMesh CgalBooleanOperations::triangleModelToSurfaceMesh(TriangleModel& model)
{
    SurfaceMesh mesh;
    std::vector<Vect3d>* verts = model.getVertices();
    std::vector<unsigned>* idxs = model.getIndices();

    if (!verts || !idxs) return mesh;

    std::vector<SurfaceMesh::Vertex_index> vindices;
    vindices.reserve(verts->size());
    for (const auto& v : *verts)
    {
        vindices.push_back(mesh.add_vertex(Point_3(v.getX(), v.getY(), v.getZ())));
    }

    size_t nFaces = idxs->size() / 3;
    for (size_t i = 0; i < nFaces; ++i)
    {
        unsigned i0 = (*idxs)[i * 3 + 0];
        unsigned i1 = (*idxs)[i * 3 + 1];
        unsigned i2 = (*idxs)[i * 3 + 2];
        if (i0 < vindices.size() && i1 < vindices.size() && i2 < vindices.size())
        {
            mesh.add_face(vindices[i0], vindices[i1], vindices[i2]);
        }
    }

    return mesh;
}

void CgalBooleanOperations::surfaceMeshToTriangleModel(const SurfaceMesh& mesh, std::vector<Triangle3d>& outTriangles)
{
    outTriangles.clear();
    std::unordered_map<SurfaceMesh::Vertex_index, size_t> vertexIndexMap;
    std::vector<Vect3d> verts;
    verts.reserve(mesh.number_of_vertices());

    size_t idx = 0;
    for (auto v : mesh.vertices())
    {
        Point_3 p = mesh.point(v);
        verts.emplace_back(p.x(), p.y(), p.z());
        vertexIndexMap[v] = idx++;
    }

    for (auto f : mesh.faces())
    {
        std::vector<size_t> vidx;
        for (auto v : CGAL::vertices_around_face(mesh.halfedge(f), mesh))
        {
            auto it = vertexIndexMap.find(v);
            if (it != vertexIndexMap.end())
                vidx.push_back(it->second);
        }
        if (vidx.size() == 3)
        {
            outTriangles.emplace_back(verts[vidx[0]], verts[vidx[1]], verts[vidx[2]]);
        }
    }
}

bool CgalBooleanOperations::loadOFFToSurfaceMesh(const std::string& path, SurfaceMesh& mesh)
{
    std::ifstream input(path);
    if (!input)
    {
        std::cerr << "[CgalBoolean] No se pudo abrir: " << path << std::endl;
        return false;
    }

    if (!(input >> mesh))
    {
        std::cerr << "[CgalBoolean] Error leyendo OFF: " << path << std::endl;
        return false;
    }

    if (!CGAL::is_triangle_mesh(mesh))
    {
        std::cerr << "[CgalBoolean] La malla no es triangulada: " << path << std::endl;
        return false;
    }

    return true;
}

bool CgalBooleanOperations::isValidForBoolean(const SurfaceMesh& mesh)
{
    if (mesh.number_of_faces() == 0)
    {
        return false;
    }
    if (!CGAL::is_triangle_mesh(mesh))
    {
        return false;
    }
    if (!CGAL::is_closed(mesh))
    {
        return false;
    }
    return true;
}

bool CgalBooleanOperations::loadOBJToSurfaceMesh(const std::string& path, SurfaceMesh& mesh)
{
    mesh.clear();
    if (!CGAL::IO::read_OBJ(path, mesh))
    {
        std::cerr << "[CgalBoolean] Error leyendo OBJ: " << path << std::endl;
        return false;
    }
    if (!CGAL::is_triangle_mesh(mesh))
    {
        std::cout << "[CgalBoolean] La malla OBJ no es puramente triangular. Triangulando..." << std::endl;
        try
        {
            PMP::triangulate_faces(mesh);
        }
        catch (...)
        {
            std::cerr << "[CgalBoolean] Error triangulando malla OBJ." << std::endl;
            return false;
        }
    }
    return mesh.number_of_vertices() > 0;
}

CgalBooleanOperations::Point_3 CgalBooleanOperations::transformPoint(const Point_3& p, const mat4& m)
{
    // GLM es column-major: m[col][row]
    double x = p.x(), y = p.y(), z = p.z();
    double nx = m[0][0] * x + m[1][0] * y + m[2][0] * z + m[3][0];
    double ny = m[0][1] * x + m[1][1] * y + m[2][1] * z + m[3][1];
    double nz = m[0][2] * x + m[1][2] * y + m[2][2] * z + m[3][2];
    double w  = m[0][3] * x + m[1][3] * y + m[2][3] * z + m[3][3];
    if (w != 0.0 && w != 1.0)
    {
        nx /= w; ny /= w; nz /= w;
    }
    return Point_3(nx, ny, nz);
}

void CgalBooleanOperations::applyModelMatrixToMeshes(const mat4& matA, const mat4& matB)
{
    if (!_hasA || !_hasB) return;
    _meshA = _meshA_orig;
    _meshB = _meshB_orig;
    for (auto v : _meshA.vertices())
    {
        _meshA.point(v) = transformPoint(_meshA.point(v), matA);
    }
    for (auto v : _meshB.vertices())
    {
        _meshB.point(v) = transformPoint(_meshB.point(v), matB);
    }
    std::cout << "[CgalBoolean] Transformaciones visuales aplicadas a las mallas de entrada." << std::endl;
}

bool CgalBooleanOperations::loadModelA(const std::string& path)
{
    _meshA.clear();
    _meshA_orig.clear();
    _hasA = false;

    if (path.size() > 4 && path.substr(path.size() - 4) == ".off")
    {
        _hasA = loadOFFToSurfaceMesh(path, _meshA);
    }
    else if (path.size() > 4 && path.substr(path.size() - 4) == ".obj")
    {
        _hasA = loadOBJToSurfaceMesh(path, _meshA);
        if (!_hasA)
        {
            std::cout << "[CgalBoolean] Fallback a Assimp para OBJ..." << std::endl;
            try
            {
                TriangleModel tm(path);
                _meshA = triangleModelToSurfaceMesh(tm);
                _hasA = _meshA.number_of_vertices() > 0;
            }
            catch (...)
            {
                std::cerr << "[CgalBoolean] Error cargando modelo A: " << path << std::endl;
                _hasA = false;
            }
        }
    }
    else
    {
        try
        {
            TriangleModel tm(path);
            _meshA = triangleModelToSurfaceMesh(tm);
            _hasA = _meshA.number_of_vertices() > 0;
        }
        catch (...)
        {
            std::cerr << "[CgalBoolean] Error cargando modelo A: " << path << std::endl;
            _hasA = false;
        }
    }

    if (_hasA)
    {
        repairAndOrientMesh(_meshA);
        _meshA_orig = _meshA;
        _validA = isValidForBoolean(_meshA);
        std::cout << "[CgalBoolean] Modelo A cargado: " << _meshA.number_of_vertices() << " vertices, " << _meshA.number_of_faces() << " caras." << std::endl;
        if (_validA)
            std::cout << "[CgalBoolean] Modelo A es VALIDO (cerrado y triangulado)." << std::endl;
        else
            std::cerr << "[CgalBoolean] Modelo A NO es valido: debe ser malla cerrada y triangulada sin agujeros." << std::endl;
    }
    else
    {
        _validA = false;
    }
    return _hasA;
}

bool CgalBooleanOperations::loadModelB(const std::string& path)
{
    _meshB.clear();
    _meshB_orig.clear();
    _hasB = false;

    if (path.size() > 4 && path.substr(path.size() - 4) == ".off")
    {
        _hasB = loadOFFToSurfaceMesh(path, _meshB);
    }
    else if (path.size() > 4 && path.substr(path.size() - 4) == ".obj")
    {
        _hasB = loadOBJToSurfaceMesh(path, _meshB);
        if (!_hasB)
        {
            std::cout << "[CgalBoolean] Fallback a Assimp para OBJ..." << std::endl;
            try
            {
                TriangleModel tm(path);
                _meshB = triangleModelToSurfaceMesh(tm);
                _hasB = _meshB.number_of_vertices() > 0;
            }
            catch (...)
            {
                std::cerr << "[CgalBoolean] Error cargando modelo B: " << path << std::endl;
                _hasB = false;
            }
        }
    }
    else
    {
        try
        {
            TriangleModel tm(path);
            _meshB = triangleModelToSurfaceMesh(tm);
            _hasB = _meshB.number_of_vertices() > 0;
        }
        catch (...)
        {
            std::cerr << "[CgalBoolean] Error cargando modelo B: " << path << std::endl;
            _hasB = false;
        }
    }

    if (_hasB)
    {
        repairAndOrientMesh(_meshB);
        _meshB_orig = _meshB;
        _validB = isValidForBoolean(_meshB);
        std::cout << "[CgalBoolean] Modelo B cargado: " << _meshB.number_of_vertices() << " vertices, " << _meshB.number_of_faces() << " caras." << std::endl;
        if (_validB)
            std::cout << "[CgalBoolean] Modelo B es VALIDO (cerrado y triangulado)." << std::endl;
        else
            std::cerr << "[CgalBoolean] Modelo B NO es valido: debe ser malla cerrada y triangulada sin agujeros." << std::endl;
    }
    else
    {
        _validB = false;
    }
    return _hasB;
}

bool CgalBooleanOperations::setModelA(TriangleModel& model)
{
    _meshA = triangleModelToSurfaceMesh(model);
    _hasA = _meshA.number_of_vertices() > 0;
    _validA = _hasA && isValidForBoolean(_meshA);
    if (_hasA && !_validA)
    {
        std::cerr << "[CgalBoolean] Advertencia: Modelo A no es una malla cerrada/triangulada." << std::endl;
    }
    return _hasA;
}

bool CgalBooleanOperations::setModelB(TriangleModel& model)
{
    _meshB = triangleModelToSurfaceMesh(model);
    _hasB = _meshB.number_of_vertices() > 0;
    _validB = _hasB && isValidForBoolean(_meshB);
    if (_hasB && !_validB)
    {
        std::cerr << "[CgalBoolean] Advertencia: Modelo B no es una malla cerrada/triangulada." << std::endl;
    }
    return _hasB;
}

static void logBooleanException(const char* opName, const std::exception& e)
{
    std::string msg = e.what();
    std::cerr << "[CgalBoolean] Excepcion en " << opName << ": " << msg << std::endl;
    if (msg.find("Unauthorized intersections") != std::string::npos)
    {
        std::cerr << "[CgalBoolean] >> Las mallas tienen intersecciones no manejables (caras coplanarias," << std::endl;
        std::cerr << "[CgalBoolean] >> aristas tangentes, o vertices muy cercanos). Prueba a mover" << std::endl;
        std::cerr << "[CgalBoolean] >> uno de los modelos ligeramente o usa geometria mas simple." << std::endl;
    }
    else if (msg.find("assertion violation") != std::string::npos)
    {
        std::cerr << "[CgalBoolean] >> Violacion de asercion interna de CGAL. Posiblemente los modelos" << std::endl;
        std::cerr << "[CgalBoolean] >> son demasiado complejos o tienen degeneraciones." << std::endl;
    }
}

bool CgalBooleanOperations::computeUnion()
{
    std::cout << "[CgalBoolean] Iniciando UNION..." << std::endl;
    if (!_hasA || !_hasB)
    {
        std::cerr << "[CgalBoolean] Faltan modelos para la operación." << std::endl;
        return false;
    }
    if (!isValidForBoolean(_meshA) || !isValidForBoolean(_meshB))
    {
        std::cerr << "[CgalBoolean] UNION abortada: uno o ambos modelos no son validos para operaciones booleanas (deben ser mallas cerradas)." << std::endl;
        return false;
    }

    if (areMeshesIdentical(_meshA, _meshB))
    {
        std::cerr << "[CgalBoolean] UNION abortada: los modelos A y B parecen ser identicos." << std::endl;
        std::cerr << "[CgalBoolean] Consejo: carga modelos diferentes que se solapen parcialmente," << std::endl;
        std::cerr << "[CgalBoolean] o usa el gizmo para mover uno de ellos ligeramente." << std::endl;
        return false;
    }

    _result.clear();
    _hasResult = false;

    SurfaceMesh meshA_copy = _meshA;
    SurfaceMesh meshB_copy = _meshB;
    SurfaceMesh out_union, out_inter, out_diff_ab, out_diff_ba;

    std::array<std::optional<SurfaceMesh*>, 4> outputs;
    outputs[0] = &out_union;
    outputs[1] = &out_inter;
    outputs[2] = &out_diff_ab;
    outputs[3] = &out_diff_ba;

    auto start = std::chrono::high_resolution_clock::now();
    std::array<bool, 4> results = { false, false, false, false };
    try
    {
        results = PMP::corefine_and_compute_boolean_operations(meshA_copy, meshB_copy, outputs);
    }
    catch (const std::exception& e)
    {
        logBooleanException("corefine_and_compute_boolean_operations (Union)", e);
        return false;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    if (!results[0])
    {
        std::cerr << "[CgalBoolean] UNION fallo: posible autointerseccion o topologia no soportada." << std::endl;
        return false;
    }

    _result = out_union;
    _hasResult = _result.number_of_faces() > 0;

    if (_hasResult)
    {
        std::cout << "[CgalBoolean] UNION completada en " << elapsed.count() << " s. "
                  << "Resultado: " << _result.number_of_vertices() << " vértices, "
                  << _result.number_of_faces() << " caras." << std::endl;
    }
    else
    {
        std::cout << "[CgalBoolean] UNION resultó vacía." << std::endl;
    }

    return _hasResult;
}

bool CgalBooleanOperations::computeIntersection()
{
    std::cout << "[CgalBoolean] Iniciando INTERSECCION..." << std::endl;
    if (!_hasA || !_hasB)
    {
        std::cerr << "[CgalBoolean] Faltan modelos para la operación." << std::endl;
        return false;
    }
    if (!isValidForBoolean(_meshA) || !isValidForBoolean(_meshB))
    {
        std::cerr << "[CgalBoolean] INTERSECCION abortada: uno o ambos modelos no son validos." << std::endl;
        return false;
    }

    if (areMeshesIdentical(_meshA, _meshB))
    {
        std::cerr << "[CgalBoolean] INTERSECCION abortada: los modelos A y B parecen ser identicos." << std::endl;
        std::cerr << "[CgalBoolean] Consejo: carga modelos diferentes que se solapen parcialmente." << std::endl;
        return false;
    }

    _result.clear();
    _hasResult = false;

    SurfaceMesh meshA_copy = _meshA;
    SurfaceMesh meshB_copy = _meshB;
    SurfaceMesh out_union, out_inter, out_diff_ab, out_diff_ba;

    std::array<std::optional<SurfaceMesh*>, 4> outputs;
    outputs[0] = &out_union;
    outputs[1] = &out_inter;
    outputs[2] = &out_diff_ab;
    outputs[3] = &out_diff_ba;

    auto start = std::chrono::high_resolution_clock::now();
    std::array<bool, 4> results = { false, false, false, false };
    try
    {
        results = PMP::corefine_and_compute_boolean_operations(meshA_copy, meshB_copy, outputs);
    }
    catch (const std::exception& e)
    {
        logBooleanException("corefine_and_compute_boolean_operations (Interseccion)", e);
        return false;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    if (!results[1])
    {
        std::cerr << "[CgalBoolean] INTERSECCION fallo." << std::endl;
        return false;
    }

    _result = out_inter;
    _hasResult = _result.number_of_faces() > 0;

    if (_hasResult)
    {
        std::cout << "[CgalBoolean] INTERSECCIÓN completada en " << elapsed.count() << " s. "
                  << "Resultado: " << _result.number_of_vertices() << " vértices, "
                  << _result.number_of_faces() << " caras." << std::endl;
    }
    else
    {
        std::cout << "[CgalBoolean] INTERSECCIÓN resultó vacía." << std::endl;
    }

    return _hasResult;
}

bool CgalBooleanOperations::computeDifferenceAB()
{
    std::cout << "[CgalBoolean] Iniciando DIFERENCIA A-B..." << std::endl;
    if (!_hasA || !_hasB)
    {
        std::cerr << "[CgalBoolean] Faltan modelos para la operación." << std::endl;
        return false;
    }
    if (!isValidForBoolean(_meshA) || !isValidForBoolean(_meshB))
    {
        std::cerr << "[CgalBoolean] DIFERENCIA A-B abortada: uno o ambos modelos no son validos." << std::endl;
        return false;
    }

    if (areMeshesIdentical(_meshA, _meshB))
    {
        std::cerr << "[CgalBoolean] DIFERENCIA A-B abortada: los modelos A y B parecen ser identicos." << std::endl;
        std::cerr << "[CgalBoolean] Consejo: carga modelos diferentes que se solapen parcialmente." << std::endl;
        return false;
    }

    _result.clear();
    _hasResult = false;

    SurfaceMesh meshA_copy = _meshA;
    SurfaceMesh meshB_copy = _meshB;
    SurfaceMesh out_union, out_inter, out_diff_ab, out_diff_ba;

    std::array<std::optional<SurfaceMesh*>, 4> outputs;
    outputs[0] = &out_union;
    outputs[1] = &out_inter;
    outputs[2] = &out_diff_ab;
    outputs[3] = &out_diff_ba;

    auto start = std::chrono::high_resolution_clock::now();
    std::array<bool, 4> results = { false, false, false, false };
    try
    {
        results = PMP::corefine_and_compute_boolean_operations(meshA_copy, meshB_copy, outputs);
    }
    catch (const std::exception& e)
    {
        logBooleanException("corefine_and_compute_boolean_operations (Diferencia A-B)", e);
        return false;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    if (!results[2])
    {
        std::cerr << "[CgalBoolean] DIFERENCIA A-B fallo." << std::endl;
        return false;
    }

    _result = out_diff_ab;
    _hasResult = _result.number_of_faces() > 0;

    if (_hasResult)
    {
        std::cout << "[CgalBoolean] DIFERENCIA A-B completada en " << elapsed.count() << " s. "
                  << "Resultado: " << _result.number_of_vertices() << " vértices, "
                  << _result.number_of_faces() << " caras." << std::endl;
    }
    else
    {
        std::cout << "[CgalBoolean] DIFERENCIA A-B resultó vacía." << std::endl;
    }

    return _hasResult;
}

bool CgalBooleanOperations::computeDifferenceBA()
{
    std::cout << "[CgalBoolean] Iniciando DIFERENCIA B-A..." << std::endl;
    if (!_hasA || !_hasB)
    {
        std::cerr << "[CgalBoolean] Faltan modelos para la operación." << std::endl;
        return false;
    }
    if (!isValidForBoolean(_meshA) || !isValidForBoolean(_meshB))
    {
        std::cerr << "[CgalBoolean] DIFERENCIA B-A abortada: uno o ambos modelos no son validos." << std::endl;
        return false;
    }

    if (areMeshesIdentical(_meshA, _meshB))
    {
        std::cerr << "[CgalBoolean] DIFERENCIA B-A abortada: los modelos A y B parecen ser identicos." << std::endl;
        std::cerr << "[CgalBoolean] Consejo: carga modelos diferentes que se solapen parcialmente." << std::endl;
        return false;
    }

    _result.clear();
    _hasResult = false;

    SurfaceMesh meshA_copy = _meshA;
    SurfaceMesh meshB_copy = _meshB;
    SurfaceMesh out_union, out_inter, out_diff_ab, out_diff_ba;

    std::array<std::optional<SurfaceMesh*>, 4> outputs;
    outputs[0] = &out_union;
    outputs[1] = &out_inter;
    outputs[2] = &out_diff_ab;
    outputs[3] = &out_diff_ba;

    auto start = std::chrono::high_resolution_clock::now();
    std::array<bool, 4> results = { false, false, false, false };
    try
    {
        results = PMP::corefine_and_compute_boolean_operations(meshA_copy, meshB_copy, outputs);
    }
    catch (const std::exception& e)
    {
        logBooleanException("corefine_and_compute_boolean_operations (Diferencia B-A)", e);
        return false;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    if (!results[3])
    {
        std::cerr << "[CgalBoolean] DIFERENCIA B-A fallo." << std::endl;
        return false;
    }

    _result = out_diff_ba;
    _hasResult = _result.number_of_faces() > 0;

    if (_hasResult)
    {
        std::cout << "[CgalBoolean] DIFERENCIA B-A completada en " << elapsed.count() << " s. "
                  << "Resultado: " << _result.number_of_vertices() << " vértices, "
                  << _result.number_of_faces() << " caras." << std::endl;
    }
    else
    {
        std::cout << "[CgalBoolean] DIFERENCIA B-A resultó vacía." << std::endl;
    }

    return _hasResult;
}

bool CgalBooleanOperations::simplifyResult(double ratio)
{
    if (!_hasResult)
    {
        std::cerr << "[CgalBoolean] No hay resultado para simplificar." << std::endl;
        return false;
    }

    if (ratio >= 1.0)
    {
        std::cout << "[CgalBoolean] Ratio=1.0: sin cambios (no se simplifica)." << std::endl;
        return true;
    }
    if (ratio <= 0.0)
    {
        std::cerr << "[CgalBoolean] Ratio invalido (<=0)." << std::endl;
        return false;
    }

    if (!CGAL::is_triangle_mesh(_result))
    {
        std::cerr << "[CgalBoolean] El resultado no es una malla triangulada. No se puede simplificar." << std::endl;
        return false;
    }

    std::cout << "[CgalBoolean] Simplificando resultado con ratio=" << ratio << " (conservar ~" << (int)(ratio*100) << "% aristas)..." << std::endl;
    std::cout << "[CgalBoolean] Antes: " << _result.number_of_vertices() << " vertices, " << _result.number_of_faces() << " caras, " << _result.number_of_edges() << " aristas." << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    SMS::Edge_count_ratio_stop_predicate<SurfaceMesh> stop(ratio);
    int r = 0;
    try
    {
        r = SMS::edge_collapse(_result, stop);
    }
    catch (const std::exception& e)
    {
        std::cerr << "[CgalBoolean] Excepcion en edge_collapse: " << e.what() << std::endl;
        return false;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "[CgalBoolean] Simplificacion completada en " << elapsed.count() << " s. "
              << r << " aristas colapsadas. "
              << "Despues: " << _result.number_of_vertices() << " vertices, "
              << _result.number_of_faces() << " caras." << std::endl;

    return true;
}

TriangleModel* CgalBooleanOperations::exportResultToTriangleModel() const
{
    if (!_hasResult)
    {
        std::cerr << "[CgalBoolean] No hay resultado para exportar." << std::endl;
        return nullptr;
    }

    // Construir directamente vertices e indices sin pasar por Triangle3d (evita deduplicacion O(n^2))
    std::vector<Vect3d> verts;
    std::vector<unsigned> idxs;
    verts.reserve(_result.number_of_vertices());
    idxs.reserve(_result.number_of_faces() * 3);

    std::unordered_map<SurfaceMesh::Vertex_index, unsigned> vertexIndexMap;
    unsigned idx = 0;
    for (auto v : _result.vertices())
    {
        Point_3 p = _result.point(v);
        verts.emplace_back(p.x(), p.y(), p.z());
        vertexIndexMap[v] = idx++;
    }

    for (auto f : _result.faces())
    {
        std::vector<unsigned> vidx;
        for (auto v : CGAL::vertices_around_face(_result.halfedge(f), _result))
        {
            auto it = vertexIndexMap.find(v);
            if (it != vertexIndexMap.end())
                vidx.push_back(it->second);
        }
        if (vidx.size() == 3)
        {
            idxs.push_back(vidx[0]);
            idxs.push_back(vidx[1]);
            idxs.push_back(vidx[2]);
        }
    }

    if (verts.empty() || idxs.empty())
    {
        std::cerr << "[CgalBoolean] El resultado no contiene triangulos." << std::endl;
        return nullptr;
    }

    return new TriangleModel(std::move(verts), std::move(idxs));
}

bool CgalBooleanOperations::saveResultToOBJ(const std::string& path) const
{
    if (!_hasResult) return false;

    std::ofstream out(path);
    if (!out.is_open())
    {
        std::cerr << "[CgalBoolean] No se pudo crear archivo OBJ: " << path << std::endl;
        return false;
    }

    out << "# Generado por CgalBooleanOperations\n";

    for (auto v : _result.vertices())
    {
        Point_3 p = _result.point(v);
        out << "v " << p.x() << " " << p.y() << " " << p.z() << "\n";
    }

    for (auto f : _result.faces())
    {
        std::vector<size_t> idxs;
        for (auto v : CGAL::vertices_around_face(_result.halfedge(f), _result))
        {
            // OBJ usa índices 1-based
            idxs.push_back(v.idx() + 1);
        }
        if (idxs.size() == 3)
        {
            out << "f " << idxs[0] << " " << idxs[1] << " " << idxs[2] << "\n";
        }
    }

    out.close();
    std::cout << "[CgalBoolean] Resultado guardado en: " << path << std::endl;
    return true;
}

bool CgalBooleanOperations::saveResultToOFF(const std::string& path) const
{
    if (!_hasResult) return false;

    std::ofstream out(path);
    if (!out.is_open())
    {
        std::cerr << "[CgalBoolean] No se pudo crear archivo OFF: " << path << std::endl;
        return false;
    }

    out << _result;
    out.close();
    std::cout << "[CgalBoolean] Resultado guardado en: " << path << std::endl;
    return true;
}

void CgalBooleanOperations::printInfoA() const
{
    std::cout << "[CgalBoolean] Modelo A: " << _meshA.number_of_vertices()
              << " vértices, " << _meshA.number_of_faces() << " caras." << std::endl;
}

void CgalBooleanOperations::printInfoB() const
{
    std::cout << "[CgalBoolean] Modelo B: " << _meshB.number_of_vertices()
              << " vértices, " << _meshB.number_of_faces() << " caras." << std::endl;
}

void CgalBooleanOperations::printInfoResult() const
{
    if (_hasResult)
    {
        std::cout << "[CgalBoolean] Resultado: " << _result.number_of_vertices()
                  << " vértices, " << _result.number_of_faces() << " caras." << std::endl;
    }
    else
    {
        std::cout << "[CgalBoolean] No hay resultado." << std::endl;
    }
}

size_t CgalBooleanOperations::getResultVertices() const
{
    return _hasResult ? _result.number_of_vertices() : 0;
}

size_t CgalBooleanOperations::getResultFaces() const
{
    return _hasResult ? _result.number_of_faces() : 0;
}
