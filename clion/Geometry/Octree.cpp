#include "stdafx.h"
#include "Octree.h"
#include "RandomUtilities.h"
#include "Ray3d.h"
#include <iostream>
#include <algorithm>

Octree::Octree(TriangleModel* bm_model, const std::string& objFile)
    : model(bm_model), optimized(true), rayos_indecision(0), nivel_maximo(0), maximo_triangulos_nodo(0)
{
    model->setOctree(this);
    
    // Calcular la caja AABB completa
    AABB fullBox;
    auto vertices = model->getVertices();
    if (vertices && !vertices->empty()) {
        Vect3d v0 = (*vertices)[0];
        fullBox = AABB(vec3(v0.getX(), v0.getY(), v0.getZ()),
                       vec3(v0.getX(), v0.getY(), v0.getZ()));
        
        for (const auto& v : *vertices) {
            Vect3d p = v;
            fullBox.update(vec3(p.getX(), p.getY(), p.getZ()));
        }
    }
    
    raiz = new NodeOctree(0, fullBox.min(), fullBox.max(), this);
    
    // Añadir todos los triángulos a la raíz
    auto faces = model->getFacesPtrs();
    for (auto tri : faces) {
        raiz->pContenidos.push_back(tri);
    }
    
    // Construir el árbol recursivamente
    buildNode(raiz);
}

Octree::~Octree()
{
    if (raiz) delete raiz;
}

void Octree::buildNode(NodeOctree* node)
{
    // Actualizar estadísticas
    if (node->nivel > nivel_maximo) {
        nivel_maximo = node->nivel;
    }
    
    // Actualizar máximo de triángulos SÓLO para las hojas
    if (node->nivel >= MAX_LEVELS || node->pContenidos.size() <= MAX_TRI_NODE) {
        if (node->pContenidos.size() > maximo_triangulos_nodo) {
            maximo_triangulos_nodo = node->pContenidos.size();
        }
    }

    if (node->nivel >= MAX_LEVELS) {
        if (node->pContenidos.size() > MAX_TRI_NODE) {
            optimized = false; // No pudimos dividir más (alcanzado MAX_LEVELS)
        }
        return;
    }
    
    if (node->pContenidos.size() <= MAX_TRI_NODE) {
        return; // Condición cumplida, no se necesita subdivisión
    }
    
    node->creaHijos();
    
    // Distribuir a los hijos
    for (int i = 0; i < 8; i++) {
        NodeOctree* child = node->hijos[i];
        for (auto tri : node->pContenidos) {
            if (child->box.AABB_tri(*tri)) {
                child->pContenidos.push_back(tri);
            }
        }
        buildNode(child);
    }
    
    // Limpiar el contenido del padre ya que se ha pasado a las hojas
    node->pContenidos.clear();
}

void Octree::classify_color()
{
    classifyNode(raiz);
}

void Octree::classifyNode(NodeOctree* node)
{
    if (node->esHoja()) {
        if (!node->pContenidos.empty()) {
            node->color = GREY; // Frontera del modelo
        } else {
            // No tiene triángulos -> BLANCO o NEGRO
            // Lanzar 2 rayos desde el centro
            vec3 centerBox = node->box.center();
            Vect3d center(centerBox.x, centerBox.y, centerBox.z);
            
            // Rayos aleatorios en lugar de ortogonales para evitar rasar caras/aristas 
            vec3 v1 = RandomUtilities::getUniformRandomInUnitSphere();
            vec3 v2 = RandomUtilities::getUniformRandomInUnitSphere();
            Vect3d dir1(v1.x, v1.y, v1.z);
            Vect3d dir2(v2.x, v2.y, v2.z);
            
            Vect3d dest1(center.getX()+dir1.getX(), center.getY()+dir1.getY(), center.getZ()+dir1.getZ());
            Vect3d dest2(center.getX()+dir2.getX(), center.getY()+dir2.getY(), center.getZ()+dir2.getZ());
            
            Ray3d ray1(center, dest1);
            Ray3d ray2(center, dest2);
            
            std::vector<Triangle3d*> testedTriangles1, testedTriangles2, testedTriangles3;
            
            bool odd1 = (countRayIntersections(raiz, ray1, testedTriangles1) % 2) != 0;
            bool odd2 = (countRayIntersections(raiz, ray2, testedTriangles2) % 2) != 0;
            
            if (odd1 && odd2) {
                node->color = BLACK;
            } else if (!odd1 && !odd2) {
                node->color = WHITE;
            } else {
                // Ley de Murphy -> 3er rayo para desempatar
                rayos_indecision++;
                vec3 v3 = RandomUtilities::getUniformRandomInUnitSphere();
                Vect3d dir3(v3.x, v3.y, v3.z);
                Vect3d dest3(center.getX()+dir3.getX(), center.getY()+dir3.getY(), center.getZ()+dir3.getZ());
                Ray3d ray3(center, dest3);
                bool odd3 = (countRayIntersections(raiz, ray3, testedTriangles3) % 2) != 0;
                
                if (odd3) node->color = BLACK;
                else node->color = WHITE;
            }
        }
    } else {
        for (int i = 0; i < 8; i++) {
            classifyNode(node->hijos[i]);
        }
    }
}

NodeOctree* Octree::findLeafRec(NodeOctree* node, const Vect3d& p)
{
    if (node->esHoja()) return node;
    
    vec3 med = node->box.center();
    
    int childIndex = 0;
    Vect3d p_copy = p;
    if (p_copy.getX() >= med.x) childIndex |= 1;
    if (p_copy.getY() >= med.y) childIndex |= 2;
    if (p_copy.getZ() >= med.z) childIndex |= 4;
    
    return findLeafRec(node->hijos[childIndex], p);
}

NodeOctree* Octree::findLeaf(const Vect3d& p)
{
    return findLeafRec(raiz, p);
}

void Octree::printStatistics() const
{
    std::cout << "--- Estadisticas del Octree ---" << std::endl;
    std::cout << "Rayos de indecision lanzados (3er rayo): " << rayos_indecision << std::endl;
    std::cout << "Nivel maximo alcanzado: " << nivel_maximo << std::endl;
    std::cout << "Maximo numero de triangulos en un nodo: " << maximo_triangulos_nodo << std::endl;
    std::cout << "-------------------------------" << std::endl;
}

int Octree::countRayIntersections(NodeOctree* node, Ray3d& ray, std::vector<Triangle3d*>& testedTriangles)
{
    if (!node->box.rayIntersects(ray)) return 0;
    
    int hits = 0;
    if (node->esHoja()) {
        for (auto tri : node->pContenidos) {
            if (std::find(testedTriangles.begin(), testedTriangles.end(), tri) == testedTriangles.end()) {
                testedTriangles.push_back(tri);
                Vect3d p;
                if (tri->ray_tri(ray, p)) {
                    hits++;
                }
            }
        }
    } else {
        for (int i = 0; i < 8; i++) {
            hits += countRayIntersections(node->hijos[i], ray, testedTriangles);
        }
    }
    return hits;
}
