#include "stdafx.h"
#include "BasicGeometry.h"
#include "PointCloud3d.h"
#include "Triangle3d.h"
#include "Segment3d.h"


PointCloud3d::PointCloud3d(): _maxPoint(INFINITY, -INFINITY, -INFINITY), _minPoint(INFINITY, INFINITY, INFINITY) 
{
}

PointCloud3d::PointCloud3d (std::vector<Vect3d>& pointCloud):_points(pointCloud), _maxPoint(INFINITY, -INFINITY, -INFINITY), _minPoint(INFINITY, INFINITY, INFINITY)
{
}

PointCloud3d::PointCloud3d(const std::string & filename): _maxPoint(-INFINITY, -INFINITY, -INFINITY), _minPoint(INFINITY, INFINITY, INFINITY)
{
	auto splitByComma = [this](std::string& string) -> std::vector<std::string>
	{
		std::stringstream ss(string);
		std::vector<std::string> result;

		while (ss.good())
		{
			std::string substr;
			getline(ss, substr, ',');
			result.push_back(substr);
		}

		return result;
	};

	std::string currentLine; 				// Línea actual del fichero.
	std::ifstream inputStream;				// Flujo de entrada.
	inputStream.open(filename.c_str());

	while (std::getline(inputStream, currentLine))
	{
		std::vector<std::string> coord = splitByComma(currentLine);

		if (coord.size() == 3)	
		{
			try {
				Vect3d point(std::stof(coord[0].c_str(), nullptr), std::stof(coord[1].c_str(), nullptr), (std::stof(coord[2].c_str(), nullptr)));
				this->addPoint(point);
			}
			catch (const std::exception& excep)
			{
				inputStream.close();

				throw excep;
			}
		}
	}
	inputStream.close();					// Cerramos fichero.
}

PointCloud3d::PointCloud3d(int size, float max_x, float max_y, float max_z): _maxPoint(-INFINITY, -INFINITY, -INFINITY), _minPoint(INFINITY, INFINITY, INFINITY)
{
	_points = std::vector<Vect3d>();

	while (size > 0)
	{
		float x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_x * 2.0f))) - max_x;
		float y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_y * 2.0f))) - max_y;
		float z = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_z * 2.0f))) - max_z;
                Vect3d val (x,y,z);
		this->addPoint(val);

		--size;
	}
}

#include <random>

PointCloud3d::PointCloud3d(int size, float radius): _maxPoint(-INFINITY, -INFINITY, -INFINITY), _minPoint(INFINITY, INFINITY, INFINITY)
{
	_points = std::vector<Vect3d>();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> distTheta(0.0, 2.0 * glm::pi<double>());
	std::uniform_real_distribution<double> distPhi(-1.0, 1.0);
	std::uniform_real_distribution<double> distR(0.0, 1.0);

	while (size > 0)
	{
		double theta = distTheta(gen);
		double cosPhi = distPhi(gen); // Para distribución uniforme en la esfera
		double sinPhi = std::sqrt(1.0 - cosPhi * cosPhi);

		double x = sinPhi * std::cos(theta);
		double y = sinPhi * std::sin(theta);
		double z = cosPhi;

		double r = radius * std::cbrt(distR(gen)); // Distribución uniforme en volumen

		Vect3d point(r * x, r * y, r * z);
		this->addPoint(point);

		--size;
	}
}

PointCloud3d::PointCloud3d(const PointCloud3d & pointCloud)
	: _points(pointCloud._points), _maxPoint(pointCloud._maxPoint), _minPoint(pointCloud._minPoint)
{
}

PointCloud3d::~PointCloud3d()
{

}

void PointCloud3d::addPoint(Vect3d & p)
{
	_points.push_back(p);
	this->updateMaxMin(_points.size() - 1);
}

AABB PointCloud3d::getAABB()
{
	return AABB(vec3(_minPoint.getX(), _minPoint.getY(), _minPoint.getZ()), vec3(_maxPoint.getX(), _maxPoint.getY(), _maxPoint.getZ()));
}

Vect3d PointCloud3d::getPoint(int pos)
{
	if ((pos >= 0) && (pos < _points.size())) {
		return _points[pos];
	}
	return Vect3d();
}

PointCloud3d& PointCloud3d::operator=(const PointCloud3d & pointCloud)
{
	if (this != &pointCloud)
	{
		_points = pointCloud._points;
		_maxPoint = pointCloud._maxPoint;
		_minPoint = pointCloud._minPoint;
	}

	return *this;
}

void PointCloud3d::save(const std::string & filename)
{
	std::ofstream file(filename);

	for (int i = 0; i < _points.size(); ++i)
	{
		file << _points[i].getX() << ", " << _points[i].getY() << ", " << _points[i].getZ() << std::endl;
	}

	file.close();
}


/// PROTECTED METHODS

void PointCloud3d::updateMaxMin(int index)
{
	Vect3d point = _points[index];

	if (point.getX() < _minPoint.getX()) { _minPoint.setX(point.getX()); _minPointIndex.setX(index); }
	if (point.getY() < _minPoint.getY()) { _minPoint.setY(point.getY()); _minPointIndex.setY(index); }
	if (point.getZ() < _minPoint.getZ()) { _minPoint.setZ(point.getZ()); _minPointIndex.setZ(index); }

	if (point.getX() > _maxPoint.getX()) { _maxPoint.setX(point.getX()); _maxPointIndex.setX(index); }
	if (point.getY() > _maxPoint.getY()) { _maxPoint.setY(point.getY()); _maxPointIndex.setY(index); }
	if (point.getZ() > _maxPoint.getZ()) { _maxPoint.setZ(point.getZ()); _maxPointIndex.setZ(index); }
}

void PointCloud3d::getMostDistanced (int &a, int &b)
{
    a = 0; b = 0;
    double maxDist = -1.0;

    for (int i = 0; i < (int)_points.size(); ++i)
    {
        for (int j = i + 1; j < (int)_points.size(); ++j)
        {
            double dist = _points[i].distance(_points[j]);
            if (dist > maxDist)
            {
                maxDist = dist;
                a = i;
                b = j;
            }
        }
    }
}

// ========================================================================================
// PRACTICA 5: Gift Wrapping 3D — Envolvente Convexa
// ========================================================================================

#include "TriangleModel.h"
#include <list>
#include <set>
#include <cmath>
#include <algorithm>

// -------------------------------------------------------
// Función auxiliar: comprobar si un punto ya está en un vector de puntos
// -------------------------------------------------------
static bool puntoEnConjunto(const std::vector<Vect3d>& conjunto, Vect3d& punto)
{
    for (size_t i = 0; i < conjunto.size(); i++)
    {
        Vect3d p = conjunto[i];
        if (p == punto) return true;
    }
    return false;
}

// -------------------------------------------------------
// Función auxiliar: buscar y eliminar un segmento de la lista frontera
// Devuelve true si lo encontró y eliminó
// -------------------------------------------------------
static bool buscarYEliminar(std::list<Segment3d>& frontera, Segment3d& seg)
{
    for (auto it = frontera.begin(); it != frontera.end(); ++it)
    {
        if (it->compare(seg))
        {
            frontera.erase(it);
            return true;
        }
    }
    return false;
}

// -------------------------------------------------------
// Función auxiliar: comprobar si un segmento existe en la lista frontera
// -------------------------------------------------------
static bool existeEnFrontera(const std::list<Segment3d>& frontera, Segment3d& seg)
{
    for (auto it = frontera.begin(); it != frontera.end(); ++it)
    {
        Segment3d s = *it;
        if (s.compare(seg))
            return true;
    }
    return false;
}

// -------------------------------------------------------
// Función auxiliar: orientar el triángulo para que su normal apunte hacia afuera
// (contraria al centro geométrico de la nube)
// -------------------------------------------------------
static Triangle3d orientarNormalHaciaAfuera(Vect3d& a, Vect3d& b, Vect3d& c, Vect3d& centroide)
{
    Triangle3d tri(a, b, c);
    Vect3d normal = tri.normal();

    // Vector desde el centroide hacia un vértice del triángulo
    Vect3d centroAvertice = a.sub(centroide);

    // Si el producto escalar es negativo, la normal apunta hacia dentro: invertir winding
    if (normal.dot(centroAvertice) < 0.0)
    {
        // Intercambiar b y c para invertir la normal
        Triangle3d triInv(a, c, b);
        return triInv;
    }
    return tri;
}

// -------------------------------------------------------
// Función auxiliar: comprobar si un triángulo ya existe en el resultado
// -------------------------------------------------------
static bool trianguloYaExiste(const std::vector<Triangle3d>& CH, Vect3d& a, Vect3d& b, Vect3d& c)
{
    for (size_t i = 0; i < CH.size(); i++)
    {
        Triangle3d t = CH[i];
        Vect3d ta = t.getA(), tb = t.getB(), tc = t.getC();
        // Comprobar todas las permutaciones cíclicas y sus inversas
        if ((ta == a && tb == b && tc == c) ||
            (ta == b && tb == c && tc == a) ||
            (ta == c && tb == a && tc == b) ||
            (ta == a && tb == c && tc == b) ||
            (ta == c && tb == b && tc == a) ||
            (ta == b && tb == a && tc == c))
        {
            return true;
        }
    }
    return false;
}


// ========================================================================================
// CH_GiftWrapping() — Versión O(n²) por iteración
// ========================================================================================
TriangleModel* PointCloud3d::CH_GiftWrapping()
{
    std::vector<Triangle3d> CH;     // Resultado: triángulos de la envolvente
    int n = (int)_points.size();

    if (n < 4)
    {
        std::cout << "Se necesitan al menos 4 puntos para calcular la envolvente convexa 3D." << std::endl;
        return nullptr;
    }

    // --- Calcular centroide de la nube para orientar normales ---
    Vect3d centroide(0, 0, 0);
    for (int i = 0; i < n; i++)
    {
        Vect3d tmp = centroide.add(_points[i]);
        centroide = tmp;
    }
    centroide = centroide.scalarMul(1.0 / n);

    // --- PASO 1: Encontrar el punto A (menor coordenada Y) ---
    int idxA = 0;
    for (int i = 1; i < n; i++)
    {
        if (_points[i].getY() < _points[idxA].getY())
            idxA = i;
        else if (_points[i].getY() == _points[idxA].getY() && _points[i].getX() < _points[idxA].getX())
            idxA = i;
    }
    Vect3d A = _points[idxA];

    // --- PASO 2: Encontrar el punto B (Jarvis 2D, proyección XZ) ---
    // Se busca el punto que forme el menor ángulo polar desde A en la proyección 2D (X, Z)
    int idxB = -1;
    double minAngle = 1e18;
    for (int i = 0; i < n; i++)
    {
        if (i == idxA) continue;
        double dx = _points[i].getX() - A.getX();
        double dy = _points[i].getY() - A.getY();
        double angle = std::atan2(dy, dx);
        if (angle < minAngle || (angle == minAngle && _points[i].distance(A) > _points[idxB].distance(A)))
        {
            minAngle = angle;
            idxB = i;
        }
    }
    Vect3d B = _points[idxB];

    // --- PASO 3: Encontrar el punto C (triángulo primigenio) en O(n²) ---
    // Para cada candidato pi, comprobar que el plano ABpi deja todos los demás puntos a un lado
    int idxC = -1;
    for (int i = 0; i < n; i++)
    {
        if (i == idxA || i == idxB) continue;

        Triangle3d triCandidate(A, B, _points[i]);
        bool todosAunLado = true;
        int ladoPrimero = 0; // 0 = sin determinar

        for (int j = 0; j < n; j++)
        {
            if (j == idxA || j == idxB || j == i) continue;
            Vect3d pj = _points[j];
            Vect3d pDiff = pj.sub(A);
            Vect3d n = triCandidate.normal();
            double dist = n.dot(pDiff);
            if (std::abs(dist) < 1e-6) continue; // Considerarlo coplanar

            int ladoActual = (dist > 0.0) ? 1 : -1;

            if (ladoPrimero == 0)
                ladoPrimero = ladoActual;
            else if (ladoActual != ladoPrimero)
            {
                todosAunLado = false;
                break;
            }
        }

        if (todosAunLado && ladoPrimero != 0)
        {
            idxC = i;
            break;
        }
    }

    if (idxC == -1)
    {
        std::cout << "No se pudo encontrar el triangulo primigenio. Posiblemente todos los puntos son coplanares." << std::endl;
        return nullptr;
    }
    Vect3d C = _points[idxC];

    // --- PASO 4: Inicializar estructuras ---
    std::list<Segment3d> BoundaryCH;        // Aristas frontera
    std::vector<Vect3d> PointsInCH;         // Puntos ya en la envolvente

    // Crear el primer triángulo orientado hacia afuera
    Triangle3d primerTri = orientarNormalHaciaAfuera(A, B, C, centroide);
    CH.push_back(primerTri);

    // Obtener los vértices del triángulo orientado
    Vect3d tA = primerTri.getA(), tB = primerTri.getB(), tC = primerTri.getC();

    // Insertar las aristas frontera
    Segment3d arAB(tA, tB), arBC(tB, tC), arCA(tC, tA);
    BoundaryCH.push_back(arAB);
    BoundaryCH.push_back(arBC);
    BoundaryCH.push_back(arCA);

    // Marcar puntos como parte de la envolvente
    PointsInCH.push_back(tA);
    PointsInCH.push_back(tB);
    PointsInCH.push_back(tC);

    // --- PASO 5: Bucle principal ---
    while (!BoundaryCH.empty())
    {
        // Sacar arista frontera D-E
        Segment3d aristaDE = BoundaryCH.front();
        BoundaryCH.pop_front();

        Vect3d D = aristaDE.getOrigin();
        Vect3d E = aristaDE.getDestination();

        // Buscar el vértice V tal que DEV deje todos los puntos a un lado
        // y DEV no esté ya en CH
        int idxV = -1;
        for (int i = 0; i < n; i++)
        {
            Vect3d pi = _points[i];
            if (pi == D || pi == E) continue;

            // Comprobar que este triángulo no existe ya
            if (trianguloYaExiste(CH, D, E, pi)) continue;

            Triangle3d triCandidate(D, E, pi);
            bool todosAunLado = true;
            int ladoPrimero = 0;

            for (int j = 0; j < n; j++)
            {
                Vect3d pj = _points[j];
                if (pj == D || pj == E || pj == pi) continue;

                Vect3d pDiff = pj.sub(D);
                Vect3d n = triCandidate.normal();
                double dist = n.dot(pDiff);
                if (std::abs(dist) < 1e-6) continue;

                int ladoActual = (dist > 0.0) ? 1 : -1;

                if (ladoPrimero == 0)
                    ladoPrimero = ladoActual;
                else if (ladoActual != ladoPrimero)
                {
                    todosAunLado = false;
                    break;
                }
            }

            if (todosAunLado)
            {
                idxV = i;
                break;
            }
        }

        if (idxV == -1) continue; // No se encontró candidato (posible degeneración)

        Vect3d V = _points[idxV];

        // Añadir el nuevo triángulo orientado hacia afuera
        Triangle3d nuevoTri = orientarNormalHaciaAfuera(D, E, V, centroide);
        CH.push_back(nuevoTri);

        // Gestionar aristas frontera
        Segment3d aristaDV(D, V);
        Segment3d aristaEV(E, V);

        if (!puntoEnConjunto(PointsInCH, V))
        {
            // V es nuevo: añadir al conjunto y añadir las dos aristas
            PointsInCH.push_back(V);
            BoundaryCH.push_back(aristaDV);
            BoundaryCH.push_back(aristaEV);
        }
        else
        {
            // V ya pertenece a la envolvente: gestionar cierre de fronteras
            bool existeDV = existeEnFrontera(BoundaryCH, aristaDV);
            bool existeEV = existeEnFrontera(BoundaryCH, aristaEV);

            if (existeDV && existeEV)
            {
                // Ambas existen: eliminar ambas (cerramos el hueco completo)
                buscarYEliminar(BoundaryCH, aristaDV);
                buscarYEliminar(BoundaryCH, aristaEV);
            }
            else if (existeDV && !existeEV)
            {
                // Solo existe D-V: eliminar D-V y añadir E-V
                buscarYEliminar(BoundaryCH, aristaDV);
                BoundaryCH.push_back(aristaEV);
            }
            else if (!existeDV && existeEV)
            {
                // Solo existe E-V: eliminar E-V y añadir D-V
                buscarYEliminar(BoundaryCH, aristaEV);
                BoundaryCH.push_back(aristaDV);
            }
            else
            {
                // Ninguna existe: añadir ambas
                BoundaryCH.push_back(aristaDV);
                BoundaryCH.push_back(aristaEV);
            }
        }
    }

    std::cout << "Gift Wrapping O(n^2): " << CH.size() << " triangulos generados." << std::endl;
    return new TriangleModel(CH);
}

