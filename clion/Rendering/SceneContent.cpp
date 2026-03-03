#include "stdafx.h"
#include "SceneContent.h"

#include "ChronoUtilities.h"
#include "InclDraw2D.h"
#include "InclDraw3D.h"
#include "InclGeom2D.h"
#include "InclGeom3D.h"
#include "PointCloud.h"
#include "RandomUtilities.h"
#include <algorithm>
#include <vector>
#include <iostream>


// ----------------------------- BUILD YOUR SCENARIO HERE -----------------------------------

void AlgGeom::SceneContent::clearScene()
{
    _model.clear();
    _sceneAABB = AABB();
}

void AlgGeom::SceneContent::buildScenario()
{
}

// ============================== PRACTICA 1A ==============================

void AlgGeom::SceneContent::buildPr1a()
{
    // ====================================================
    // EJERCICIO 1: Nube de 200 puntos aleatorios
    // ====================================================
    PointCloud cloud;
    float radioNube = 5.0f;
    for (int i = 0; i < 200; i++)
    {
        vec3 p = RandomUtilities::getUniformRandomInUnitDisk();
        cloud.addPoint(Point(p.x * radioNube, p.y * radioNube));
    }
    cloud.save("../../nube_puntos.txt");

    // Dibujar nube
    this->addNewModel((new DrawPointCloud(cloud))->setPointColor(vec4(0.0f, 0.0f, 1.0f, 1.0f))->overrideModelName());

    // ====================================================
    // EJERCICIO 2: Dos segmentos con interseccion propia
    // ====================================================
    int numPuntos = static_cast<int>(cloud.size());
    bool encontrado = false;

    for (int intento = 0; intento < 5000 && !encontrado; intento++)
    {
        int i1 = RandomUtilities::getUniformRandomInt(0, numPuntos);
        int i2 = RandomUtilities::getUniformRandomInt(0, numPuntos);
        int i3 = RandomUtilities::getUniformRandomInt(0, numPuntos);
        int i4 = RandomUtilities::getUniformRandomInt(0, numPuntos);

        if (i1 == i2 || i3 == i4 || i1 == i3 || i1 == i4 || i2 == i3 || i2 == i4)
            continue;

        SegmentLine seg1(cloud.getPoint(i1), cloud.getPoint(i2));
        SegmentLine seg2(cloud.getPoint(i3), cloud.getPoint(i4));

        if (seg1.segmentIntersection(seg2))
        {
            this->addNewModel((new DrawSegment(seg1))->setLineColor(vec4(1.0f, 0.0f, 0.0f, 1.0f))->overrideModelName()->setLineWidth(2.0f));
            this->addNewModel((new DrawSegment(seg2))->setLineColor(vec4(1.0f, 0.0f, 0.0f, 1.0f))->overrideModelName()->setLineWidth(2.0f));
            encontrado = true;
        }
    }

    // ====================================================
    // EJERCICIO 3: Recta (azul) y Rayo (verde)
    // ====================================================
    {
        int ri1 = RandomUtilities::getUniformRandomInt(0, numPuntos);
        int ri2 = RandomUtilities::getUniformRandomInt(0, numPuntos);
        while (ri2 == ri1) ri2 = RandomUtilities::getUniformRandomInt(0, numPuntos);

        Line* recta = new Line(cloud.getPoint(ri1), cloud.getPoint(ri2));
        this->addNewModel((new DrawLine(*recta))->setLineColor(vec4(0.0f, 0.0f, 1.0f, 1.0f))->overrideModelName()->setLineWidth(2.0f));
        delete recta;
    }
    {
        int ri1 = RandomUtilities::getUniformRandomInt(0, numPuntos);
        int ri2 = RandomUtilities::getUniformRandomInt(0, numPuntos);
        while (ri2 == ri1) ri2 = RandomUtilities::getUniformRandomInt(0, numPuntos);

        RayLine* rayo = new RayLine(cloud.getPoint(ri1), cloud.getPoint(ri2));
        this->addNewModel((new DrawRay(*rayo))->setLineColor(vec4(0.0f, 1.0f, 0.0f, 1.0f))->overrideModelName()->setLineWidth(2.0f));
        delete rayo;
    }

    // ====================================================
    // EJERCICIO 4: Poligono valido con 5 puntos aleatorios
    // ====================================================
    {
        std::vector<int> indices;
        while (indices.size() < 5)
        {
            int idx = RandomUtilities::getUniformRandomInt(0, numPuntos);
            bool repetido = false;
            for (int k : indices)
                if (k == idx) { repetido = true; break; }
            if (!repetido) indices.push_back(idx);
        }

        std::vector<std::pair<double, Point>> puntosConAngulo;
        double cx = 0.0, cy = 0.0;
        for (int idx : indices)
        {
            Point pt = cloud.getPoint(idx);
            cx += pt.getX();
            cy += pt.getY();
        }
        cx /= 5.0;
        cy /= 5.0;

        for (int idx : indices)
        {
            Point pt = cloud.getPoint(idx);
            double angulo = std::atan2(pt.getY() - cy, pt.getX() - cx);
            puntosConAngulo.push_back({angulo, pt});
        }

        std::sort(puntosConAngulo.begin(), puntosConAngulo.end(),
            [](const std::pair<double, Point>& a, const std::pair<double, Point>& b) {
                return a.first < b.first;
            });

        Polygon* poligono = new Polygon;
        for (auto& par : puntosConAngulo)
            poligono->add(par.second);

        this->addNewModel((new DrawPolygon(*poligono))->setTriangleColor(vec4(0.0f, 1.0f, 1.0f, 1.0f))->overrideModelName());
        delete poligono;
    }

    // ====================================================
    // EJERCICIO 5: Triangulo , inscrito , circunscrito
    // ====================================================
    {
        int ti1 = RandomUtilities::getUniformRandomInt(0, numPuntos);
        int ti2 = RandomUtilities::getUniformRandomInt(0, numPuntos);
        while (ti2 == ti1) ti2 = RandomUtilities::getUniformRandomInt(0, numPuntos);
        int ti3 = RandomUtilities::getUniformRandomInt(0, numPuntos);
        while (ti3 == ti1 || ti3 == ti2) ti3 = RandomUtilities::getUniformRandomInt(0, numPuntos);

        Vect2d va(cloud.getPoint(ti1).getX(), cloud.getPoint(ti1).getY());
        Vect2d vb(cloud.getPoint(ti2).getX(), cloud.getPoint(ti2).getY());
        Vect2d vc(cloud.getPoint(ti3).getX(), cloud.getPoint(ti3).getY());

        Triangle triangulo(va, vb, vc);
        this->addNewModel((new DrawTriangle(triangulo))->setTriangleColor(vec4(1.0f, 0.0f, 0.0f, 1.0f))->overrideModelName());

        Circle inscrito = triangulo.getInscribed();
        this->addNewModel((new DrawCircle(inscrito))->setLineColor(vec4(0.0f, 0.0f, 0.0f, 1.0f))->setTriangleColor(vec4(0.0f, 0.0f, 0.0f, 0.0f))->overrideModelName()->setLineWidth(2.0f));

        Circle circunscrito = triangulo.getCirumscribed();
        this->addNewModel((new DrawCircle(circunscrito))->setLineColor(vec4(0.0f, 0.0f, 0.0f, 1.0f))->setTriangleColor(vec4(0.0f, 0.0f, 0.0f, 0.0f))->overrideModelName()->setLineWidth(2.0f));
    }
}

// ============================== PRACTICA 1B ==============================

void AlgGeom::SceneContent::buildPr1b()
{
    // ====================================================
    // EJERCICIO 1: Definir primitivas con intersecciones
    // L1-L2, S1-S2, S1-R1, L1-P, R2-P
    // ====================================================

    SegmentLine S1(Point(-3, -1), Point(3, 3));     // Segmento diagonal
    SegmentLine S2(Point(-2, 3), Point(2, -1));      // Segmento que cruza S1
    RayLine R1(Point(-4, 1), Point(4, 1));           // Rayo horizontal que cruza S1
    RayLine R2(Point(4, -2), Point(-4, 4));          // Rayo que cruza el poligono
    Line L1(Point(-5, -3), Point(5, 5));             // Recta diagonal
    Line L2(Point(-5, 4), Point(5, -2));             // Recta que cruza L1

    // Poligono P1
    Polygon P1;
    P1.add(Point(-2, -2));
    P1.add(Point(2, -2));
    P1.add(Point(2, 2));
    P1.add(Point(-2, 2));


    this->addNewModel((new DrawSegment(S1))->setLineColor(vec4(1.0f, 0.0f, 0.0f, 1.0f))->overrideModelName()->setLineWidth(2.0f));   // rojo
    this->addNewModel((new DrawSegment(S2))->setLineColor(vec4(1.0f, 0.5f, 0.0f, 1.0f))->overrideModelName()->setLineWidth(2.0f));   // naranja
    this->addNewModel((new DrawRay(R1))->setLineColor(vec4(0.0f, 1.0f, 0.0f, 1.0f))->overrideModelName()->setLineWidth(2.0f));       // verde
    this->addNewModel((new DrawRay(R2))->setLineColor(vec4(0.0f, 0.8f, 0.0f, 1.0f))->overrideModelName()->setLineWidth(2.0f));       // verde oscuro
    this->addNewModel((new DrawLine(L1))->setLineColor(vec4(0.0f, 0.0f, 1.0f, 1.0f))->overrideModelName()->setLineWidth(2.0f));      // azul
    this->addNewModel((new DrawLine(L2))->setLineColor(vec4(0.5f, 0.0f, 1.0f, 1.0f))->overrideModelName()->setLineWidth(2.0f));      // violeta
    this->addNewModel((new DrawPolygon(P1))->setTriangleColor(vec4(0.0f, 1.0f, 1.0f, 0.3f))->overrideModelName());                   // cyan

    Vect2d intersection;

    // L1-L2
    if (L1.intersects(L2, intersection))
    {
        Point pi(intersection.getX(), intersection.getY());
        this->addNewModel((new DrawPoint(pi))->setPointColor(vec4(1.0f, 1.0f, 0.0f, 1.0f))->overrideModelName()->setPointSize(8.0f));
    }

    // S1-S2
    if (S1.intersects(S2, intersection))
    {
        Point pi(intersection.getX(), intersection.getY());
        this->addNewModel((new DrawPoint(pi))->setPointColor(vec4(1.0f, 1.0f, 0.0f, 1.0f))->overrideModelName()->setPointSize(8.0f));
    }

    // S1-R1
    if (S1.intersects(R1, intersection))
    {
        Point pi(intersection.getX(), intersection.getY());
        this->addNewModel((new DrawPoint(pi))->setPointColor(vec4(1.0f, 1.0f, 0.0f, 1.0f))->overrideModelName()->setPointSize(8.0f));
    }

    // L1-P1
    if (P1.intersects(L1, intersection))
    {
        Point pi(intersection.getX(), intersection.getY());
        this->addNewModel((new DrawPoint(pi))->setPointColor(vec4(1.0f, 1.0f, 0.0f, 1.0f))->overrideModelName()->setPointSize(8.0f));
    }

    // R2-P1
    if (P1.intersects(R2, intersection))
    {
        Point pi(intersection.getX(), intersection.getY());
        this->addNewModel((new DrawPoint(pi))->setPointColor(vec4(1.0f, 1.0f, 0.0f, 1.0f))->overrideModelName()->setPointSize(8.0f));
    }

    // ====================================================
    // EJERCICIO 2: Distancias vertices del poligono
    // ====================================================
    std::cout << "\n========================================" << std::endl;
    std::cout << "DISTANCIAS VERTICES DEL POLIGONO P1" << std::endl;
    std::cout << "========================================" << std::endl;
    for (int i = 0; i < P1.getNumVertices(); i++)
    {
        Vertex v = P1.getVertexAt(i);
        Vect2d vp(v.getX(), v.getY());

        std::cout << "--- Vertice " << i << " (" << v.getX() << ", " << v.getY() << ") ---" << std::endl;
        std::cout << "  Distancia al Segmento S1: " << S1.distPointSegment(vp) << std::endl;
        std::cout << "  Distancia al Segmento S2: " << S2.distPointSegment(vp) << std::endl;
        std::cout << "  Distancia al Rayo R1:     " << R1.distPointRay(vp) << std::endl;
        std::cout << "  Distancia al Rayo R2:     " << R2.distPointRay(vp) << std::endl;
        std::cout << "  Distancia a la Recta L1:  " << L1.distancePointLine(vp) << std::endl;
        std::cout << "  Distancia a la Recta L2:  " << L2.distancePointLine(vp) << std::endl;
        std::cout << "----------------------------------------" << std::endl;
    }

    // ====================================================
    // EJERCICIO 3: Circulos y relaciones
    // ====================================================
    Circle C1(Point(0, 0), 3.0);
    Circle C2(Point(4, 0), 2.0);

    // Dibujar circulos
    this->addNewModel((new DrawCircle(C1))->setLineColor(vec4(1.0f, 0.0f, 1.0f, 1.0f))->setTriangleColor(vec4(0.0f, 0.0f, 0.0f, 0.0f))->overrideModelName()->setLineWidth(2.0f));
    this->addNewModel((new DrawCircle(C2))->setLineColor(vec4(0.0f, 1.0f, 1.0f, 1.0f))->setTriangleColor(vec4(0.0f, 0.0f, 0.0f, 0.0f))->overrideModelName()->setLineWidth(2.0f));

    std::cout << "\n========================================" << std::endl;
    std::cout << "RELACIONES CIRCULOS - LINEAS Y CIRCULOS " << std::endl;
    std::cout << "========================================" << std::endl;

    // Relacion entre circulos
    RelationCircles cc = C1.relacionaCir(C2);

    std::cout << "RELACION C1-C2: " << Circle::getStringEnum(cc) << std::endl;

    // Relacion circulos con rectas
    RelationCircleLine cl = C1.relacionaLine(L1);
    std::cout << "RELACION C1-L1: " << Circle::getStringEnum(cl) << std::endl;

    cl = C1.relacionaLine(L2);
    std::cout << "RELACION C1-L2: " << Circle::getStringEnum(cl) << std::endl;

    cl = C2.relacionaLine(L1);
    std::cout << "RELACION C2-L1: " << Circle::getStringEnum(cl) << std::endl;

    cl = C2.relacionaLine(L2);
    std::cout << "RELACION C2-L2: " << Circle::getStringEnum(cl) << std::endl;

    // ====================================================
    // EJERCICIO 4: Intersecciones circulo-primitivas
    // ====================================================
    Vect2d p1, p2;

    // C1 con L1
    RelationCircleLine relC1L1 = C1.intersect(L1, p1, p2);
    if (relC1L1 == INTERSECT)
    {
        Point pi1(p1.getX(), p1.getY()); Point pi2(p2.getX(), p2.getY());
        this->addNewModel((new DrawPoint(pi1))->setPointColor(vec4(1.0f, 0.0f, 0.0f, 1.0f))->overrideModelName()->setPointSize(10.0f));
        this->addNewModel((new DrawPoint(pi2))->setPointColor(vec4(1.0f, 0.0f, 0.0f, 1.0f))->overrideModelName()->setPointSize(10.0f));
    }
    else if (relC1L1 == TANGENTS)
    {
        Point pi1(p1.getX(), p1.getY());
        this->addNewModel((new DrawPoint(pi1))->setPointColor(vec4(1.0f, 0.0f, 0.0f, 1.0f))->overrideModelName()->setPointSize(10.0f));
    }

    // C1 con L2
    RelationCircleLine relC1L2 = C1.intersect(L2, p1, p2);
    if (relC1L2 == INTERSECT)
    {
        Point pi1(p1.getX(), p1.getY()); Point pi2(p2.getX(), p2.getY());
        this->addNewModel((new DrawPoint(pi1))->setPointColor(vec4(1.0f, 0.0f, 0.0f, 1.0f))->overrideModelName()->setPointSize(10.0f));
        this->addNewModel((new DrawPoint(pi2))->setPointColor(vec4(1.0f, 0.0f, 0.0f, 1.0f))->overrideModelName()->setPointSize(10.0f));
    }
    else if (relC1L2 == TANGENTS)
    {
        Point pi1(p1.getX(), p1.getY());
        this->addNewModel((new DrawPoint(pi1))->setPointColor(vec4(1.0f, 0.0f, 0.0f, 1.0f))->overrideModelName()->setPointSize(10.0f));
    }

    // C2 con L1
    RelationCircleLine relC2L1 = C2.intersect(L1, p1, p2);
    if (relC2L1 == INTERSECT)
    {
        Point pi1(p1.getX(), p1.getY()); Point pi2(p2.getX(), p2.getY());
        this->addNewModel((new DrawPoint(pi1))->setPointColor(vec4(0.0f, 1.0f, 0.0f, 1.0f))->overrideModelName()->setPointSize(10.0f));
        this->addNewModel((new DrawPoint(pi2))->setPointColor(vec4(0.0f, 1.0f, 0.0f, 1.0f))->overrideModelName()->setPointSize(10.0f));
    }
    else if (relC2L1 == TANGENTS)
    {
        Point pi1(p1.getX(), p1.getY());
        this->addNewModel((new DrawPoint(pi1))->setPointColor(vec4(0.0f, 1.0f, 0.0f, 1.0f))->overrideModelName()->setPointSize(10.0f));
    }

    // C2 con L2
    RelationCircleLine relC2L2 = C2.intersect(L2, p1, p2);
    if (relC2L2 == INTERSECT)
    {
        Point pi1(p1.getX(), p1.getY()); Point pi2(p2.getX(), p2.getY());
        this->addNewModel((new DrawPoint(pi1))->setPointColor(vec4(0.0f, 1.0f, 0.0f, 1.0f))->overrideModelName()->setPointSize(10.0f));
        this->addNewModel((new DrawPoint(pi2))->setPointColor(vec4(0.0f, 1.0f, 0.0f, 1.0f))->overrideModelName()->setPointSize(10.0f));
    }
    else if (relC2L2 == TANGENTS)
    {
        Point pi1(p1.getX(), p1.getY());
        this->addNewModel((new DrawPoint(pi1))->setPointColor(vec4(0.0f, 1.0f, 0.0f, 1.0f))->overrideModelName()->setPointSize(10.0f));
    }

    // C1 con S1
    RelationCircleLine relC1S1 = C1.intersect(S1, p1, p2);
    if (relC1S1 == INTERSECT)
    {
        Point pi1(p1.getX(), p1.getY()); Point pi2(p2.getX(), p2.getY());
        this->addNewModel((new DrawPoint(pi1))->setPointColor(vec4(1.0f, 1.0f, 0.0f, 1.0f))->overrideModelName()->setPointSize(10.0f));
        this->addNewModel((new DrawPoint(pi2))->setPointColor(vec4(1.0f, 1.0f, 0.0f, 1.0f))->overrideModelName()->setPointSize(10.0f));
    }
    else if (relC1S1 == TANGENTS)
    {
        Point pi1(p1.getX(), p1.getY());
        this->addNewModel((new DrawPoint(pi1))->setPointColor(vec4(1.0f, 1.0f, 0.0f, 1.0f))->overrideModelName()->setPointSize(10.0f));
    }

    // C1 con R1
    RelationCircleLine relC1R1 = C1.intersect(R1, p1, p2);
    if (relC1R1 == INTERSECT)
    {
        Point pi1(p1.getX(), p1.getY()); Point pi2(p2.getX(), p2.getY());
        this->addNewModel((new DrawPoint(pi1))->setPointColor(vec4(1.0f, 0.5f, 0.5f, 1.0f))->overrideModelName()->setPointSize(10.0f));
        this->addNewModel((new DrawPoint(pi2))->setPointColor(vec4(1.0f, 0.5f, 0.5f, 1.0f))->overrideModelName()->setPointSize(10.0f));
    }
    else if (relC1R1 == TANGENTS)
    {
        Point pi1(p1.getX(), p1.getY());
        this->addNewModel((new DrawPoint(pi1))->setPointColor(vec4(1.0f, 0.5f, 0.5f, 1.0f))->overrideModelName()->setPointSize(10.0f));
    }
}


// ============================== PRACTICA 2 ==============================

// Subclass to access protected members of Model3D
namespace AlgGeom {
    class DrawPrimitive3D : public Model3D {
    public:
        DrawPrimitive3D() : Model3D() {}
        void addComp(Component* comp) { _components.push_back(std::unique_ptr<Component>(comp)); }
        void buildV(Component* comp) { buildVao(comp); }
    };
}

// Helper: create a Model3D* with a single 3D point
static AlgGeom::Model3D* makePoint3D(float x, float y, float z)
{
    using namespace AlgGeom;
    DrawPrimitive3D* model = new DrawPrimitive3D();
    Model3D::Component* comp = new Model3D::Component;
    comp->_vertices.push_back(VAO::Vertex{ vec3(x, y, z) });
    comp->_indices[VAO::IBO_POINT].push_back(0);
    comp->completeTopology();
    model->addComp(comp);
    model->buildV(comp);
    return model;
}

// Helper: create a Model3D* with a 3D line segment (two endpoints)
static AlgGeom::Model3D* makeLine3D(float x1, float y1, float z1, float x2, float y2, float z2)
{
    using namespace AlgGeom;
    DrawPrimitive3D* model = new DrawPrimitive3D();
    Model3D::Component* comp = new Model3D::Component;
    comp->_vertices.push_back(VAO::Vertex{ vec3(x1, y1, z1) });
    comp->_vertices.push_back(VAO::Vertex{ vec3(x2, y2, z2) });
    comp->_indices[VAO::IBO_LINE].insert(comp->_indices[VAO::IBO_LINE].end(), { 0, 1 });
    comp->completeTopology();
    model->addComp(comp);
    model->buildV(comp);
    return model;
}

// Helper: create a Model3D* with a 3D triangle (three points)
static AlgGeom::Model3D* makeTriangle3D(float x1, float y1, float z1,
                                         float x2, float y2, float z2,
                                         float x3, float y3, float z3)
{
    using namespace AlgGeom;
    DrawPrimitive3D* model = new DrawPrimitive3D();
    Model3D::Component* comp = new Model3D::Component;
    vec3 n = glm::normalize(glm::cross(vec3(x2-x1,y2-y1,z2-z1), vec3(x3-x1,y3-y1,z3-z1)));
    comp->_vertices.push_back(VAO::Vertex{ vec3(x1, y1, z1), n });
    comp->_vertices.push_back(VAO::Vertex{ vec3(x2, y2, z2), n });
    comp->_vertices.push_back(VAO::Vertex{ vec3(x3, y3, z3), n });
    comp->_indices[VAO::IBO_TRIANGLE].insert(comp->_indices[VAO::IBO_TRIANGLE].end(), { 0, 1, 2, RESTART_PRIMITIVE_INDEX });
    comp->completeTopology();
    model->addComp(comp);
    model->buildV(comp);
    return model;
}

void AlgGeom::SceneContent::buildPr2a()
{
    // ===========================================================
    // PR2 A: Point Cloud 3D, Line/Ray/Segment, distances, AABB
    // ===========================================================
    std::cout << "\n============================================" << std::endl;
    std::cout << "PRACTICA 2A" << std::endl;
    std::cout << "============================================" << std::endl;

    // 1. Create point cloud of 50 random 3D points
    PointCloud3d cloud(50, 5.0f, 5.0f, 5.0f);

    // Draw point cloud
    for (int i = 0; i < (int)cloud.size(); i++)
    {
        Vect3d p = cloud.getPoint(i);
        this->addNewModel(makePoint3D((float)p.getX(), (float)p.getY(), (float)p.getZ())
            ->setPointColor(vec3(0.0f, 0.5f, 1.0f))->overrideModelName()->setPointSize(5.0f));
    }

    // 2. Create Line, Ray, Segment from random cloud points
    int n = (int)cloud.size();
    Vect3d pA = cloud.getPoint(0), pB = cloud.getPoint(1);
    Vect3d pC = cloud.getPoint(2), pD = cloud.getPoint(3);
    Vect3d pE = cloud.getPoint(4), pF = cloud.getPoint(5);

    Line3d L1(pA, pB);
    Ray3d R1(pC, pD);
    Segment3d S1(pE, pF);

    // Draw Line (simulate infinite line with t = +-100)
    Vect3d lDir = pB.sub(pA);
    Vect3d lDirScaled = lDir.scalarMul(100.0);
    Vect3d lStart = pA.sub(lDirScaled);
    Vect3d lEnd = pA.add(lDirScaled);
    this->addNewModel(makeLine3D((float)lStart.getX(), (float)lStart.getY(), (float)lStart.getZ(),
                                  (float)lEnd.getX(), (float)lEnd.getY(), (float)lEnd.getZ())
        ->setLineColor(vec3(0.0f, 0.0f, 1.0f))->overrideModelName()->setLineWidth(2.0f));

    // Draw Ray
    Vect3d rDir = pD.sub(pC);
    Vect3d rDirScaled = rDir.scalarMul(100.0);
    Vect3d rEnd = pC.add(rDirScaled);
    this->addNewModel(makeLine3D((float)pC.getX(), (float)pC.getY(), (float)pC.getZ(),
                                  (float)rEnd.getX(), (float)rEnd.getY(), (float)rEnd.getZ())
        ->setLineColor(vec3(0.0f, 1.0f, 0.0f))->overrideModelName()->setLineWidth(2.0f));

    // Draw Segment
    this->addNewModel(makeLine3D((float)pE.getX(), (float)pE.getY(), (float)pE.getZ(),
                                  (float)pF.getX(), (float)pF.getY(), (float)pF.getZ())
        ->setLineColor(vec3(1.0f, 0.0f, 0.0f))->overrideModelName()->setLineWidth(2.0f));

    // 3. Distances
    Vect3d V1 = cloud.getPoint(6);
    Vect3d V2 = cloud.getPoint(7);

    std::cout << "Distancia V1 a L1: " << L1.distance(V1) << std::endl;
    std::cout << "Distancia V1 a S1: " << S1.distance(V1) << std::endl;
    std::cout << "Distancia V2 a R1: " << R1.distance(V2) << std::endl;

    // Draw V1 and V2 larger
    this->addNewModel(makePoint3D((float)V1.getX(), (float)V1.getY(), (float)V1.getZ())
        ->setPointColor(vec3(1.0f, 1.0f, 0.0f))->overrideModelName()->setPointSize(10.0f));
    this->addNewModel(makePoint3D((float)V2.getX(), (float)V2.getY(), (float)V2.getZ())
        ->setPointColor(vec3(1.0f, 0.5f, 0.0f))->overrideModelName()->setPointSize(10.0f));

    // 4. Normal line to L1 through V2
    Line3d normalL = L1.normalLine(V2);
    Vect3d nOrig = normalL.getOrigin();
    Vect3d nDest = normalL.getDestination();
    this->addNewModel(makeLine3D((float)nOrig.getX(), (float)nOrig.getY(), (float)nOrig.getZ(),
                                  (float)nDest.getX(), (float)nDest.getY(), (float)nDest.getZ())
        ->setLineColor(vec3(1.0f, 0.0f, 1.0f))->overrideModelName()->setLineWidth(2.0f));

    // 5. AABB
    AABB aabb = cloud.getAABB();
    // Draw AABB edges
    vec3 mn = aabb.min(), mx = aabb.max();
    vec3 corners[8] = {
        vec3(mn.x,mn.y,mn.z), vec3(mx.x,mn.y,mn.z), vec3(mx.x,mx.y,mn.z), vec3(mn.x,mx.y,mn.z),
        vec3(mn.x,mn.y,mx.z), vec3(mx.x,mn.y,mx.z), vec3(mx.x,mx.y,mx.z), vec3(mn.x,mx.y,mx.z)
    };
    int edges[12][2] = {{0,1},{1,2},{2,3},{3,0},{4,5},{5,6},{6,7},{7,4},{0,4},{1,5},{2,6},{3,7}};
    for (auto& e : edges)
    {
        this->addNewModel(makeLine3D(corners[e[0]].x,corners[e[0]].y,corners[e[0]].z,
                                      corners[e[1]].x,corners[e[1]].y,corners[e[1]].z)
            ->setLineColor(vec3(0.8f, 0.8f, 0.8f))->overrideModelName()->setLineWidth(1.0f));
    }
}

void AlgGeom::SceneContent::buildPr2b()
{
    // ===========================================================
    // PR2 B: Planes, intersections, reflection, equidistant plane
    // ===========================================================
    std::cout << "\n============================================" << std::endl;
    std::cout << "PRACTICA 2B" << std::endl;
    std::cout << "============================================" << std::endl;

    // Create point cloud
    PointCloud3d cloud(50, 5.0f, 5.0f, 5.0f);
    AABB aabb = cloud.getAABB();

    // Draw cloud
    for (int i = 0; i < (int)cloud.size(); i++)
    {
        Vect3d p = cloud.getPoint(i);
        this->addNewModel(makePoint3D((float)p.getX(), (float)p.getY(), (float)p.getZ())
            ->setPointColor(vec3(0.0f, 0.5f, 1.0f))->overrideModelName()->setPointSize(5.0f));
    }

    // 1. Top plane of AABB
    vec3 mx = aabb.max(), mn = aabb.min();
    Vect3d topP1(mn.x, mx.y, mn.z), topP2(mx.x, mx.y, mn.z), topP3(mx.x, mx.y, mx.z);
    Plane planeP(topP1, topP2, topP3, true);

    // Draw top plane as two triangles
    Vect3d topP4(mn.x, mx.y, mx.z);
    this->addNewModel(makeTriangle3D((float)topP1.getX(),(float)topP1.getY(),(float)topP1.getZ(),
                                      (float)topP2.getX(),(float)topP2.getY(),(float)topP2.getZ(),
                                      (float)topP3.getX(),(float)topP3.getY(),(float)topP3.getZ())
        ->setTriangleColor(vec4(0.5f, 0.5f, 0.5f, 0.3f))->overrideModelName());
    this->addNewModel(makeTriangle3D((float)topP1.getX(),(float)topP1.getY(),(float)topP1.getZ(),
                                      (float)topP3.getX(),(float)topP3.getY(),(float)topP3.getZ(),
                                      (float)topP4.getX(),(float)topP4.getY(),(float)topP4.getZ())
        ->setTriangleColor(vec4(0.5f, 0.5f, 0.5f, 0.3f))->overrideModelName());

    // Find V3: closest point to plane P
    double minDist = 1e18;
    int v3Idx = 0;
    for (int i = 0; i < (int)cloud.size(); i++)
    {
        Vect3d pt = cloud.getPoint(i);
        double d = planeP.distance(pt);
        if (d < minDist) { minDist = d; v3Idx = i; }
    }
    Vect3d V3 = cloud.getPoint(v3Idx);
    std::cout << "V3 (mas cercano a PlanoP): (" << V3.getX() << ", " << V3.getY() << ", " << V3.getZ() << ")" << std::endl;

    this->addNewModel(makePoint3D((float)V3.getX(), (float)V3.getY(), (float)V3.getZ())
        ->setPointColor(vec3(1.0f, 0.0f, 0.0f))->overrideModelName()->setPointSize(10.0f));

    // 2. Plane A from 3 random points, intersecting P
    Vect3d pA = cloud.getPoint(0), pB = cloud.getPoint(10), pC = cloud.getPoint(20);
    Plane planeA(pA, pB, pC, true);

    // Draw plane A as a triangle
    this->addNewModel(makeTriangle3D((float)pA.getX(),(float)pA.getY(),(float)pA.getZ(),
                                      (float)pB.getX(),(float)pB.getY(),(float)pB.getZ(),
                                      (float)pC.getX(),(float)pC.getY(),(float)pC.getZ())
        ->setTriangleColor(vec4(0.0f, 0.8f, 0.8f, 0.3f))->overrideModelName());

    // Intersection of P and A
    Line3d interLine;
    if (planeP.intersect(planeA, interLine))
    {
        Vect3d iOrig = interLine.getOrigin();
        Vect3d iDest = interLine.getDestination();
        Vect3d iDir = iDest.sub(iOrig);
        Vect3d iDirScaled = iDir.scalarMul(20.0);
        Vect3d iStart = iOrig.sub(iDirScaled);
        Vect3d iEnd = iOrig.add(iDirScaled);
        this->addNewModel(makeLine3D((float)iStart.getX(),(float)iStart.getY(),(float)iStart.getZ(),
                                      (float)iEnd.getX(),(float)iEnd.getY(),(float)iEnd.getZ())
            ->setLineColor(vec3(1.0f, 1.0f, 0.0f))->overrideModelName()->setLineWidth(3.0f));
        std::cout << "Interseccion P-A: linea" << std::endl;
    }

    // 3. Line L3 intersecting plane A
    Vect3d l3a = cloud.getPoint(30), l3b = cloud.getPoint(40);
    Line3d L3(l3a, l3b);
    Vect3d interPoint;
    if (planeA.intersect(L3, interPoint))
    {
        this->addNewModel(makePoint3D((float)interPoint.getX(),(float)interPoint.getY(),(float)interPoint.getZ())
            ->setPointColor(vec3(1.0f, 1.0f, 0.0f))->overrideModelName()->setPointSize(10.0f));
        std::cout << "L3-PlanoA interseccion: (" << interPoint.getX() << ", " << interPoint.getY() << ", " << interPoint.getZ() << ")" << std::endl;
    }

    // Draw L3
    Vect3d l3Dir = l3b.sub(l3a);
    Vect3d l3DirScaled = l3Dir.scalarMul(50.0);
    Vect3d l3Start = l3a.sub(l3DirScaled);
    Vect3d l3End = l3a.add(l3DirScaled);
    this->addNewModel(makeLine3D((float)l3Start.getX(),(float)l3Start.getY(),(float)l3Start.getZ(),
                                  (float)l3End.getX(),(float)l3End.getY(),(float)l3End.getZ())
        ->setLineColor(vec3(1.0f, 0.0f, 1.0f))->overrideModelName()->setLineWidth(2.0f));

    // 4. Reflect V3 across plane P
    Vect3d reflected = planeP.reflectedPoint(V3);
    std::cout << "V3 reflejado: (" << reflected.getX() << ", " << reflected.getY() << ", " << reflected.getZ() << ")" << std::endl;
    this->addNewModel(makePoint3D((float)reflected.getX(),(float)reflected.getY(),(float)reflected.getZ())
        ->setPointColor(vec3(0.0f, 1.0f, 0.0f))->overrideModelName()->setPointSize(10.0f));

    // 5. Equidistant plane between the two most distant points
    int idxA, idxB;
    cloud.getMostDistanced(idxA, idxB);
    Vect3d mostA = cloud.getPoint(idxA), mostB = cloud.getPoint(idxB);
    std::cout << "Puntos mas distantes: " << idxA << " y " << idxB << std::endl;
    std::cout << "  A: (" << mostA.getX() << ", " << mostA.getY() << ", " << mostA.getZ() << ")" << std::endl;
    std::cout << "  B: (" << mostB.getX() << ", " << mostB.getY() << ", " << mostB.getZ() << ")" << std::endl;

    // Draw those two points
    this->addNewModel(makePoint3D((float)mostA.getX(),(float)mostA.getY(),(float)mostA.getZ())
        ->setPointColor(vec3(1.0f, 0.0f, 0.0f))->overrideModelName()->setPointSize(12.0f));
    this->addNewModel(makePoint3D((float)mostB.getX(),(float)mostB.getY(),(float)mostB.getZ())
        ->setPointColor(vec3(1.0f, 0.0f, 0.0f))->overrideModelName()->setPointSize(12.0f));

    // Compute equidistant plane
    Vect3d eqNormal;
    float eqD;
    mostA.getPlane(mostB, eqNormal, eqD);
    std::cout << "Plano equidistante: n=(" << eqNormal.getX() << ", " << eqNormal.getY() << ", " << eqNormal.getZ() << "), d=" << eqD << std::endl;

    // Draw equidistant plane: build from midpoint + perpendicular vectors
    double midX = (mostA.getX() + mostB.getX()) / 2.0;
    double midY = (mostA.getY() + mostB.getY()) / 2.0;
    double midZ = (mostA.getZ() + mostB.getZ()) / 2.0;
    Vect3d mid(midX, midY, midZ);

    // Perpendicular vectors
    double na = eqNormal.getX(), nb = eqNormal.getY(), nc = eqNormal.getZ();
    Vect3d perp1;
    if (std::abs(na) > 0.001 || std::abs(nb) > 0.001)
        perp1 = Vect3d(-nb, na, 0);
    else
        perp1 = Vect3d(1, 0, 0);
    Vect3d perp2 = eqNormal.xProduct(perp1);

    double scale = 3.0;
    Vect3d sp1 = perp1.scalarMul(scale), sp2 = perp2.scalarMul(scale);
    Vect3d t1 = mid.add(sp1); Vect3d ep1 = t1.add(sp2);
    Vect3d t2 = mid.sub(sp1); Vect3d ep2 = t2.add(sp2);
    Vect3d t3 = mid.sub(sp1); Vect3d ep3 = t3.sub(sp2);
    Vect3d t4 = mid.add(sp1); Vect3d ep4 = t4.sub(sp2);

    this->addNewModel(makeTriangle3D((float)ep1.getX(),(float)ep1.getY(),(float)ep1.getZ(),
                                      (float)ep2.getX(),(float)ep2.getY(),(float)ep2.getZ(),
                                      (float)ep3.getX(),(float)ep3.getY(),(float)ep3.getZ())
        ->setTriangleColor(vec4(1.0f, 0.5f, 0.0f, 0.3f))->overrideModelName());
    this->addNewModel(makeTriangle3D((float)ep1.getX(),(float)ep1.getY(),(float)ep1.getZ(),
                                      (float)ep3.getX(),(float)ep3.getY(),(float)ep3.getZ(),
                                      (float)ep4.getX(),(float)ep4.getY(),(float)ep4.getZ())
        ->setTriangleColor(vec4(1.0f, 0.5f, 0.0f, 0.3f))->overrideModelName());
}

void AlgGeom::SceneContent::buildPr2c()
{
    // ===========================================================
    // PR2 C: Classify points by plane side
    // ===========================================================
    std::cout << "\n============================================" << std::endl;
    std::cout << "PRACTICA 2C" << std::endl;
    std::cout << "============================================" << std::endl;

    PointCloud3d cloud(50, 5.0f, 5.0f, 5.0f);

    // Plane A from 3 cloud points
    Vect3d pA = cloud.getPoint(0), pB = cloud.getPoint(10), pC = cloud.getPoint(20);
    Plane planeA(pA, pB, pC, true);

    // Draw plane A
    this->addNewModel(makeTriangle3D((float)pA.getX(),(float)pA.getY(),(float)pA.getZ(),
                                      (float)pB.getX(),(float)pB.getY(),(float)pB.getZ(),
                                      (float)pC.getX(),(float)pC.getY(),(float)pC.getZ())
        ->setTriangleColor(vec4(0.5f, 0.5f, 0.5f, 0.3f))->overrideModelName());

    // Color points: red = positive, blue = negative, yellow = coplanar
    Triangle3d triA(pA, pB, pC);
    for (int i = 0; i < (int)cloud.size(); i++)
    {
        Vect3d pt = cloud.getPoint(i);
        Triangle3d::PointPosition pos = triA.classify(pt);
        vec3 color;
        if (pos == Triangle3d::PointPosition::POSITIVE) color = vec3(1.0f, 0.0f, 0.0f);
        else if (pos == Triangle3d::PointPosition::NEGATIVE) color = vec3(0.0f, 0.0f, 1.0f);
        else color = vec3(1.0f, 1.0f, 0.0f);

        this->addNewModel(makePoint3D((float)pt.getX(), (float)pt.getY(), (float)pt.getZ())
            ->setPointColor(color)->overrideModelName()->setPointSize(6.0f));
    }
}

void AlgGeom::SceneContent::buildPr2d()
{
    // ===========================================================
    // PR2 D: Sorteo #6 (line-polygon) and #8 (segment-polygon)
    // ===========================================================
    std::cout << "\n============================================" << std::endl;
    std::cout << "PRACTICA 2D - Sorteo #6 y #8" << std::endl;
    std::cout << "============================================" << std::endl;

    // Define a 3D polygon (square in the XZ plane at y = 0)
    std::vector<Vect3d> polygon;
    Vect3d pp1(-3, 0, -3), pp2(3, 0, -3), pp3(3, 0, 3), pp4(-3, 0, 3);
    polygon.push_back(pp1);
    polygon.push_back(pp2);
    polygon.push_back(pp3);
    polygon.push_back(pp4);

    // Draw polygon as two triangles
    this->addNewModel(makeTriangle3D((float)pp1.getX(),(float)pp1.getY(),(float)pp1.getZ(),
                                      (float)pp2.getX(),(float)pp2.getY(),(float)pp2.getZ(),
                                      (float)pp3.getX(),(float)pp3.getY(),(float)pp3.getZ())
        ->setTriangleColor(vec4(0.0f, 0.8f, 0.8f, 0.3f))->overrideModelName());
    this->addNewModel(makeTriangle3D((float)pp1.getX(),(float)pp1.getY(),(float)pp1.getZ(),
                                      (float)pp3.getX(),(float)pp3.getY(),(float)pp3.getZ(),
                                      (float)pp4.getX(),(float)pp4.getY(),(float)pp4.getZ())
        ->setTriangleColor(vec4(0.0f, 0.8f, 0.8f, 0.3f))->overrideModelName());

    // #6: Line-Polygon intersection
    Vect3d lineA(0, -5, 0), lineB(0, 5, 0); // vertical line through center
    Line3d testLine(lineA, lineB);

    // Draw line
    this->addNewModel(makeLine3D((float)lineA.getX(),(float)lineA.getY(),(float)lineA.getZ(),
                                  (float)lineB.getX(),(float)lineB.getY(),(float)lineB.getZ())
        ->setLineColor(vec3(0.0f, 0.0f, 1.0f))->overrideModelName()->setLineWidth(2.0f));

    Vect3d interPt;
    if (Plane::intersectLine3dPolygon(testLine, polygon, interPt))
    {
        std::cout << "Sorteo #6: Recta INTERSECTA poligono en (" << interPt.getX() << ", " << interPt.getY() << ", " << interPt.getZ() << ")" << std::endl;
        this->addNewModel(makePoint3D((float)interPt.getX(),(float)interPt.getY(),(float)interPt.getZ())
            ->setPointColor(vec3(1.0f, 1.0f, 0.0f))->overrideModelName()->setPointSize(12.0f));
    }
    else
    {
        std::cout << "Sorteo #6: Recta NO intersecta poligono" << std::endl;
    }

    // #8: Segment-Polygon intersection
    Vect3d segA(1, -3, 1), segB(1, 3, 1); // vertical segment
    Segment3d testSeg(segA, segB);

    // Draw segment
    this->addNewModel(makeLine3D((float)segA.getX(),(float)segA.getY(),(float)segA.getZ(),
                                  (float)segB.getX(),(float)segB.getY(),(float)segB.getZ())
        ->setLineColor(vec3(1.0f, 0.0f, 0.0f))->overrideModelName()->setLineWidth(2.0f));

    Vect3d interPt2;
    if (Plane::intersectSegment3dPolygon(testSeg, polygon, interPt2))
    {
        std::cout << "Sorteo #8: Segmento INTERSECTA poligono en (" << interPt2.getX() << ", " << interPt2.getY() << ", " << interPt2.getZ() << ")" << std::endl;
        this->addNewModel(makePoint3D((float)interPt2.getX(),(float)interPt2.getY(),(float)interPt2.getZ())
            ->setPointColor(vec3(0.0f, 1.0f, 0.0f))->overrideModelName()->setPointSize(12.0f));
    }
    else
    {
        std::cout << "Sorteo #8: Segmento NO intersecta poligono" << std::endl;
    }

    // Test with a segment that doesn't reach the polygon
    Vect3d segC(1, 5, 1), segD(1, 2, 1); // above polygon, doesn't reach y=0
    Segment3d testSeg2(segC, segD);
    this->addNewModel(makeLine3D((float)segC.getX(),(float)segC.getY(),(float)segC.getZ(),
                                  (float)segD.getX(),(float)segD.getY(),(float)segD.getZ())
        ->setLineColor(vec3(0.5f, 0.5f, 0.0f))->overrideModelName()->setLineWidth(2.0f));

    Vect3d interPt3;
    if (Plane::intersectSegment3dPolygon(testSeg2, polygon, interPt3))
        std::cout << "Sorteo #8 (test2): Segmento INTERSECTA (inesperado)" << std::endl;
    else
        std::cout << "Sorteo #8 (test2): Segmento NO intersecta poligono (correcto - no alcanza)" << std::endl;
}


// ------------------------------------------------------------------------------------------


AlgGeom::SceneContent::SceneContent()
{
}

AlgGeom::SceneContent::~SceneContent()
{
	_camera.clear();
	_model.clear();
}

void AlgGeom::SceneContent::addNewCamera(ApplicationState* appState)
{
	_camera.push_back(std::make_unique<Camera>(appState->_viewportSize.x, appState->_viewportSize.y, true));
}

void AlgGeom::SceneContent::addNewModel(Model3D* model)
{	
	_sceneAABB.update(model->getAABB());
	_model.push_back(std::unique_ptr<Model3D>(model));
}

AlgGeom::Model3D* AlgGeom::SceneContent::getModel(Model3D::Component* component)
{
	for (auto& model : _model)
	{
		if (model->belongsModel(component))
			return model.get();
	}

	return nullptr;
}
