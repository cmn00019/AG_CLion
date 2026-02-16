#include "stdafx.h"
#include "SceneContent.h"

#include "ChronoUtilities.h"
#include "InclDraw2D.h"
#include "InclGeom2D.h"
#include "PointCloud.h"
#include "RandomUtilities.h"
#include <algorithm>
#include <vector>


// ----------------------------- BUILD YOUR SCENARIO HERE -----------------------------------

void AlgGeom::SceneContent::buildScenario()
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

    // Dibujar nube en amarillo dorado
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
