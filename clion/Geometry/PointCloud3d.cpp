#include "stdafx.h"
#include "PointCloud3d.h"
#include "Triangle3d.h"
#include "Segment3d.h"
#include "TriangleModel.h"
#include <list>
#include <set>
#include <map>
#include <cmath>
#include <iostream>
#include <chrono>
#include <random>

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
		while (ss.good()) { std::string substr; getline(ss, substr, ','); result.push_back(substr); }
		return result;
	};
	std::string currentLine;
	std::ifstream inputStream(filename.c_str());
	while (std::getline(inputStream, currentLine)) {
		std::vector<std::string> coord = splitByComma(currentLine);
		if (coord.size() == 3) {
			try { Vect3d point(std::stof(coord[0]), std::stof(coord[1]), std::stof(coord[2])); this->addPoint(point); }
			catch (...) {}
		}
	}
	inputStream.close();
}

PointCloud3d::PointCloud3d(int size, float max_x, float max_y, float max_z): _maxPoint(-INFINITY, -INFINITY, -INFINITY), _minPoint(INFINITY, INFINITY, INFINITY)
{
	while (size-- > 0) {
		float x = static_cast<float>(rand()) / (RAND_MAX / (max_x * 2.0f)) - max_x;
		float y = static_cast<float>(rand()) / (RAND_MAX / (max_y * 2.0f)) - max_y;
		float z = static_cast<float>(rand()) / (RAND_MAX / (max_z * 2.0f)) - max_z;
		Vect3d val(x, y, z); this->addPoint(val);
	}
}

PointCloud3d::PointCloud3d(int size, float radius): _maxPoint(-INFINITY, -INFINITY, -INFINITY), _minPoint(INFINITY, INFINITY, INFINITY)
{
	std::random_device rd; std::mt19937 gen(rd());
	std::uniform_real_distribution<double> distTheta(0.0, 2.0 * 3.14159265358979323846);
	std::uniform_real_distribution<double> distPhi(-1.0, 1.0);
	std::uniform_real_distribution<double> distR(0.0, 1.0);
	while (size-- > 0) {
		double theta = distTheta(gen), cosPhi = distPhi(gen), sinPhi = std::sqrt(1.0 - cosPhi * cosPhi);
		double x = sinPhi * std::cos(theta), y = sinPhi * std::sin(theta), z = cosPhi;
		double r = radius * std::cbrt(distR(gen));
		Vect3d point(r * x, r * y, r * z); this->addPoint(point);
	}
}

PointCloud3d::PointCloud3d(const PointCloud3d & pointCloud) : _points(pointCloud._points), _maxPoint(pointCloud._maxPoint), _minPoint(pointCloud._minPoint) {}
PointCloud3d::~PointCloud3d() {}

void PointCloud3d::addPoint(Vect3d & p) { _points.push_back(p); this->updateMaxMin((int)_points.size() - 1); }
AABB PointCloud3d::getAABB() { return AABB(vec3(_minPoint.getX(), _minPoint.getY(), _minPoint.getZ()), vec3(_maxPoint.getX(), _maxPoint.getY(), _maxPoint.getZ())); }
Vect3d PointCloud3d::getPoint(int pos) { if (pos >= 0 && pos < (int)_points.size()) return _points[pos]; return Vect3d(); }

PointCloud3d& PointCloud3d::operator=(const PointCloud3d & pointCloud) {
	if (this != &pointCloud) { _points = pointCloud._points; _maxPoint = pointCloud._maxPoint; _minPoint = pointCloud._minPoint; }
	return *this;
}

void PointCloud3d::save(const std::string & filename) {
	std::ofstream file(filename);
	for (auto& p : _points) file << p.getX() << ", " << p.getY() << ", " << p.getZ() << std::endl;
	file.close();
}

void PointCloud3d::updateMaxMin(int index) {
	Vect3d point = _points[index];
	if (point.getX() < _minPoint.getX()) { _minPoint.setX(point.getX()); _minPointIndex.setX(index); }
	if (point.getY() < _minPoint.getY()) { _minPoint.setY(point.getY()); _minPointIndex.setY(index); }
	if (point.getZ() < _minPoint.getZ()) { _minPoint.setZ(point.getZ()); _minPointIndex.setZ(index); }
	if (point.getX() > _maxPoint.getX()) { _maxPoint.setX(point.getX()); _maxPointIndex.setX(index); }
	if (point.getY() > _maxPoint.getY()) { _maxPoint.setY(point.getY()); _maxPointIndex.setY(index); }
	if (point.getZ() > _maxPoint.getZ()) { _maxPoint.setZ(point.getZ()); _maxPointIndex.setZ(index); }
}

void PointCloud3d::getMostDistanced(int &a, int &b) {
    a = 0; b = 0; double maxDist = -1.0;
    for (int i = 0; i < (int)_points.size(); ++i) {
        for (int j = i + 1; j < (int)_points.size(); ++j) {
            double dist = _points[i].distance(_points[j]);
            if (dist > maxDist) { maxDist = dist; a = i; b = j; }
        }
    }
}

static bool puntoEnConjunto(const std::vector<Vect3d>& conjunto, Vect3d& punto) {
    for (const auto& p : conjunto) if (p == punto) return true;
    return false;
}

static bool buscarYEliminar(std::list<Segment3d>& frontera, Segment3d& seg) {
    for (auto it = frontera.begin(); it != frontera.end(); ++it) {
        if (it->compare(seg)) { frontera.erase(it); return true; }
    }
    return false;
}

static bool existeEnFrontera(const std::list<Segment3d>& frontera, Segment3d& seg) {
    for (const auto& s : frontera) if (const_cast<Segment3d&>(s).compare(seg)) return true;
    return false;
}

static Triangle3d orientarNormalHaciaAfuera(Vect3d& a, Vect3d& b, Vect3d& c, Vect3d& centroide) {
    Triangle3d tri(a, b, c);
    if (tri.normal().dot(a.sub(centroide)) < 0.0) return Triangle3d(a, c, b);
    return tri;
}

// Ordena 3 índices para usarlos como clave única de un triángulo
static std::tuple<int,int,int> sortTriIdx(int a, int b, int c) {
    if (a > b) std::swap(a, b);
    if (b > c) std::swap(b, c);
    if (a > b) std::swap(a, b);
    return std::make_tuple(a, b, c);
}

// Busca el índice del punto más cercano a 'target' en el vector 'pts'
static int buscarIndice(const std::vector<Vect3d>& pts, Vect3d& target) {
    for (int i = 0; i < (int)pts.size(); i++) if (pts[i] == target) return i;
    return -1;
}

TriangleModel* PointCloud3d::CH_GiftWrapping() {
    int nOrig = (int)_points.size(); if (nOrig < 4) return nullptr;

    // Deduplicación para evitar crasheos con vértices dobles (ej. Ajax)
    std::map<std::tuple<double,double,double>, int> coordMap;
    std::vector<Vect3d> pts;
    for (int i = 0; i < nOrig; i++) {
        auto key = std::make_tuple(_points[i].getX(), _points[i].getY(), _points[i].getZ());
        if (coordMap.find(key) == coordMap.end()) {
            coordMap[key] = (int)pts.size();
            pts.push_back(_points[i]);
        }
    }
    int n = (int)pts.size();
    if (n < 4) return nullptr;

    std::vector<Triangle3d> CH;
    Vect3d centroide(0, 0, 0); for (auto& p : pts) centroide = centroide.add(p); centroide = centroide.scalarMul(1.0 / n);
    int idxA = 0;
    for (int i = 1; i < n; i++) if (pts[i].getY() < pts[idxA].getY() || (pts[i].getY() == pts[idxA].getY() && pts[i].getX() < pts[idxA].getX())) idxA = i;
    Vect3d A = pts[idxA];
    int idxB = -1; double minAngle = 1e18;
    for (int i = 0; i < n; i++) {
        if (i == idxA) continue;
        double dx = pts[i].getX() - A.getX(), dy = pts[i].getY() - A.getY(), angle = std::atan2(dy, dx);
        if (angle < minAngle) { minAngle = angle; idxB = i; }
    }
    Vect3d B = pts[idxB];
    int idxC = -1;
    for (int i = 0; i < n; i++) {
        if (i == idxA || i == idxB) continue;
        Triangle3d tri(A, B, pts[i]); bool allOnSide = true; int side = 0;
        for (int j = 0; j < n; j++) {
            if (j == idxA || j == idxB || j == i) continue;
            double d = tri.normal().dot(pts[j].sub(A));
            if (std::abs(d) < 1e-6) continue;
            int s = (d > 0) ? 1 : -1;
            if (side == 0) side = s; else if (side != s) { allOnSide = false; break; }
        }
        if (allOnSide && side != 0) { idxC = i; break; }
    }
    if (idxC == -1) return nullptr;
    std::list<Segment3d> Boundary; std::vector<Vect3d> PointsIn;
    
    // Caché de triángulos usando índices enteros (limpio y eficiente)
    std::set<std::tuple<int, int, int>> triCache;
    triCache.insert(sortTriIdx(idxA, idxB, idxC));

    Triangle3d first = orientarNormalHaciaAfuera(A, B, pts[idxC], centroide);
    CH.push_back(first);
    Boundary.push_back(Segment3d(first.getA(), first.getB())); Boundary.push_back(Segment3d(first.getB(), first.getC())); Boundary.push_back(Segment3d(first.getC(), first.getA()));
    PointsIn.push_back(first.getA()); PointsIn.push_back(first.getB()); PointsIn.push_back(first.getC());
    int iter = 0;
    while (!Boundary.empty()) {
        iter++;
        //if (iter % 10 == 0) std::cout << "[DEBUG LENTO] Iter: " << iter << " | Frontera: " << Boundary.size() << " | Triangulos: " << CH.size() << std::endl;
        Segment3d seg = Boundary.front(); Boundary.pop_front(); Vect3d D = seg.getOrigin(), E = seg.getDestination();
        int idxD = buscarIndice(pts, D), idxE = buscarIndice(pts, E);
        int idxV = -1;
        
        for (int i = 0; i < n; i++) {
            if (i == idxD || i == idxE) continue;
            if (triCache.count(sortTriIdx(idxD, idxE, i))) continue;
            
            Vect3d nC = E.sub(D).xProduct(pts[i].sub(D));
            double modSq = nC.dot(nC);
            if (modSq < 1e-18) continue;
            nC = nC.scalarMul(1.0 / std::sqrt(modSq));
            
            double dD = nC.dot(D);
            
            bool allOnSide = true; int side = 0;
            for (int j = 0; j < n; j++) {
                if (j == i) continue;
                double d = nC.dot(pts[j]) - dD;
                if (std::abs(d) < 1e-6) continue;
                int s = (d > 0) ? 1 : -1;
                if (side == 0) side = s; else if (side != s) { allOnSide = false; break; }
            }
            if (allOnSide && side != 0) { idxV = i; break; }
        }
        if (idxV == -1) continue;
        Vect3d V = pts[idxV]; 
        Triangle3d newTri = orientarNormalHaciaAfuera(D, E, V, centroide);
        CH.push_back(newTri);
        triCache.insert(sortTriIdx(idxD, idxE, idxV));
        
        Segment3d aDV(D, V), aEV(E, V);
        if (!puntoEnConjunto(PointsIn, V)) { PointsIn.push_back(V); Boundary.push_back(aDV); Boundary.push_back(aEV); }
        else {
            if (existeEnFrontera(Boundary, aDV)) buscarYEliminar(Boundary, aDV); else Boundary.push_back(aDV);
            if (existeEnFrontera(Boundary, aEV)) buscarYEliminar(Boundary, aEV); else Boundary.push_back(aEV);
        }
    }
    std::cout << "Envolvente O(n^2): " << CH.size() << " triangulos." << std::endl;
    return new TriangleModel(CH);
}

TriangleModel* PointCloud3d::CH_GiftWrapping_Optimized() {
    int nOrig = (int)_points.size(); if (nOrig < 4) return nullptr;

    // Deduplicar puntos (modelos 3D tienen vértices repetidos)
    std::map<std::tuple<double,double,double>, int> coordMap;
    std::vector<Vect3d> pts;
    for (int i = 0; i < nOrig; i++) {
        auto key = std::make_tuple(_points[i].getX(), _points[i].getY(), _points[i].getZ());
        if (coordMap.find(key) == coordMap.end()) {
            coordMap[key] = (int)pts.size();
            pts.push_back(_points[i]);
        }
    }
    int n = (int)pts.size();
    if (n < 4) return nullptr;

    std::vector<Triangle3d> CH;
    Vect3d centroide(0, 0, 0);
    for (const auto& p : pts) centroide = centroide.add(p);
    centroide = centroide.scalarMul(1.0 / n);

    // Buscar primer triángulo
    int idxA = 0;
    for (int i = 1; i < n; i++) if (pts[i].getY() < pts[idxA].getY() || (pts[i].getY() == pts[idxA].getY() && pts[i].getX() < pts[idxA].getX())) idxA = i;
    int idxB = -1;
    for (int i = 0; i < n; i++) {
        if (i == idxA) continue;
        Vect3d vAB = pts[i].sub(pts[idxA]), n2D(-vAB.getY(), vAB.getX(), 0); bool all = true; int s = 0;
        for (int j = 0; j < n; j++) {
            if (j == idxA || j == i) continue;
            double dot = n2D.getX() * (pts[j].getX() - pts[idxA].getX()) + n2D.getY() * (pts[j].getY() - pts[idxA].getY());
            if (std::abs(dot) < 1e-10) continue;
            int cur = (dot > 0) ? 1 : -1;
            if (s == 0) s = cur; else if (s != cur) { all = false; break; }
        }
        if (all && s != 0) { idxB = i; break; }
    }
    if (idxB == -1) return nullptr;
    int idxC = -1;
    for (int i = 0; i < n; i++) {
        if (i == idxA || i == idxB) continue;
        Vect3d v1 = pts[idxB].sub(pts[idxA]), v2 = pts[i].sub(pts[idxA]);
        Vect3d nC = v1.xProduct(v2);
        if (nC.dot(nC) < 1e-18) continue;
        bool all = true; int s = 0;
        for (int j = 0; j < n; j++) {
            if (j == idxA || j == idxB || j == i) continue;
            double dot = nC.dot(pts[j].sub(pts[idxA]));
            if (std::abs(dot) < 1e-10) continue;
            int cur = (dot > 0) ? 1 : -1;
            if (s == 0) s = cur; else if (s != cur) { all = false; break; }
        }
        if (all && s != 0) { idxC = i; break; }
    }
    if (idxC == -1) return nullptr;
    
    Triangle3d first = orientarNormalHaciaAfuera(pts[idxA], pts[idxB], pts[idxC], centroide);
    CH.push_back(first);
    
    int va = idxA, vb = idxB, vc = idxC;
    Vect3d expN = pts[idxB].sub(pts[idxA]).xProduct(pts[idxC].sub(pts[idxA]));
    if (first.normal().dot(expN) < 0) { std::swap(va, vb); }

    // Pre-extraer coordenadas en arrays planos para acceso directo sin getX/Y/Z()
    std::vector<double> px(n), py(n), pz(n);
    for (int i = 0; i < n; i++) { px[i] = pts[i].getX(); py[i] = pts[i].getY(); pz[i] = pts[i].getZ(); }

    // Frontera y mapa de vértices opuestos
    std::set<std::pair<int, int>> Boundary;
    auto addE = [&](int u, int v) { if (Boundary.count({v, u})) Boundary.erase({v, u}); else Boundary.insert({u, v}); };
    addE(va, vb); addE(vb, vc); addE(vc, va);
    
    auto getK = [](int u, int v) { return u < v ? std::make_pair(u, v) : std::make_pair(v, u); };
    std::map<std::pair<int, int>, int> opV; 
    opV[getK(va, vb)] = vc; opV[getK(vb, vc)] = va; opV[getK(vc, va)] = vb;


    // Bucle principal
    int iter = 0;
    while (!Boundary.empty()) {
        iter++;
        //if (iter % 100 == 0) std::cout << "[DEBUG OPT] Iter: " << iter << " | Frontera: " << Boundary.size() << std::endl;
        
        auto edgeIt = Boundary.begin();
        int D_idx = edgeIt->first, E_idx = edgeIt->second;
        Boundary.erase(edgeIt);
        
        if (!opV.count(getK(D_idx, E_idx))) continue;
        int idxP = opV[getK(D_idx, E_idx)];
        
        Vect3d D = pts[D_idx], E = pts[E_idx], P = pts[idxP];
        Vect3d eV = E.sub(D);
        Vect3d vPD = P.sub(D);
        Vect3d nB = eV.xProduct(vPD);
        Vect3d yA = eV.xProduct(nB);
        
        // Extraer componentes para evitar llamadas a getX/Y/Z en el bucle
        double Dx = px[D_idx], Dy = py[D_idx], Dz = pz[D_idx];
        double evx = eV.getX(), evy = eV.getY(), evz = eV.getZ();
        double nbx = nB.getX(), nby = nB.getY(), nbz = nB.getZ();
        double yax = yA.getX(), yay = yA.getY(), yaz = yA.getZ();
        
        int best = -1;
        double minAngle = 1e18;

        for (int i = 0; i < n; i++) {
            if (i == D_idx || i == E_idx || i == idxP) continue;
            
            double dx = px[i] - Dx, dy = py[i] - Dy, dz = pz[i] - Dz;
            // Producto vectorial diff x eV inlinado
            double nx = dy * evz - dz * evy;
            double ny = dz * evx - dx * evz;
            double nz = dx * evy - dy * evx;
            double modSq = nx*nx + ny*ny + nz*nz;
            if (modSq < 1e-18) continue;
            
            double cX = nx*nbx + ny*nby + nz*nbz;
            double cY = nx*yax + ny*yay + nz*yaz;
            double angle = std::atan2(cY, cX); 
            
            if (angle < minAngle) {
                best = i;
                minAngle = angle;
            }
        }
        
        if (best != -1) {
            Triangle3d newTri(pts[E_idx], pts[D_idx], pts[best]);
            CH.push_back(newTri);
            addE(D_idx, best); addE(best, E_idx);
            opV[getK(D_idx, best)] = E_idx; 
            opV[getK(best, E_idx)] = D_idx;
        }
    }
    std::cout << "Envolvente O(n): " << CH.size() << " triangulos." << std::endl;
    return new TriangleModel(CH);
}

