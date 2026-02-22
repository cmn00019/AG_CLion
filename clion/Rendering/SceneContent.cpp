#include "stdafx.h"
#include "SceneContent.h"

#include "ChronoUtilities.h"
#include "InclDraw2D.h"
#include "InclGeom2D.h"
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
    // EJERCICIO 1: Nube de 200 puntos aleatorios (disco)
    // ====================================================
    PointCloud cloud;
    float radioNube = 5.0f;
    for (int i = 0; i < 200; i++)
    {
        vec3 p = RandomUtilities::getUniformRandomInUnitDisk();
        cloud.addPoint(Point(p.x * radioNube, p.y * radioNube));
    }
    cloud.save("../../nube_puntos.txt");

    // Dibujar nube en azul
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
    // EJERCICIO 5: Triangulo + inscrito + circunscrito
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

    // Definir primitivas que se crucen apropiadamente
    SegmentLine S1(Point(-3, -1), Point(3, 3));     // Segmento diagonal
    SegmentLine S2(Point(-2, 3), Point(2, -1));      // Segmento que cruza S1
    RayLine R1(Point(-4, 1), Point(4, 1));           // Rayo horizontal que cruza S1
    RayLine R2(Point(4, -2), Point(-4, 4));          // Rayo que cruza el poligono
    Line L1(Point(-5, -3), Point(5, 5));             // Recta diagonal
    Line L2(Point(-5, 4), Point(5, -2));             // Recta que cruza L1

    // Poligono P1 (cuadrado para facilitar intersecciones)
    Polygon P1;
    P1.add(Point(-2, -2));
    P1.add(Point(2, -2));
    P1.add(Point(2, 2));
    P1.add(Point(-2, 2));

    // Dibujar primitivas
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
    for (int i = 0; i < P1.getNumVertices(); i++)
    {
        Vertex v = P1.getVertexAt(i);
        Vect2d vp(v.getX(), v.getY());

        S1.distPointSegment(vp);
        S2.distPointSegment(vp);
        R1.distPointRay(vp);
        R2.distPointRay(vp);
        L1.distancePointLine(vp);
        L2.distancePointLine(vp);
    }

    // ====================================================
    // EJERCICIO 3: Circulos y relaciones
    // ====================================================
    Circle C1(Point(0, 0), 3.0);
    Circle C2(Point(4, 0), 2.0);

    // Dibujar circulos
    this->addNewModel((new DrawCircle(C1))->setLineColor(vec4(1.0f, 0.0f, 1.0f, 1.0f))->setTriangleColor(vec4(0.0f, 0.0f, 0.0f, 0.0f))->overrideModelName()->setLineWidth(2.0f));
    this->addNewModel((new DrawCircle(C2))->setLineColor(vec4(0.0f, 1.0f, 1.0f, 1.0f))->setTriangleColor(vec4(0.0f, 0.0f, 0.0f, 0.0f))->overrideModelName()->setLineWidth(2.0f));

    // Relacion entre circulos
    C1.relacionaCir(C2);

    // Relacion circulos con rectas
    C1.relacionaLine(L1);
    C1.relacionaLine(L2);
    C2.relacionaLine(L1);
    C2.relacionaLine(L2);

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
