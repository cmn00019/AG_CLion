#include "stdafx.h"
#include "Octree.h"
#include "RandomUtilities.h"
#include "Ray3d.h"
#include <iostream>
#include <algorithm>
#include <unordered_set>

Octree::Octree(TriangleModel* bm_model, const std::string& objFile)
    : model(bm_model), optimized(true), rayos_indecision(0), nivel_maximo(0), maximo_triangulos_nodo(0), nodos_no_optimizados(0)
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
    
    // Actualizar máximo de triángulos
    if (node->nivel >= MAX_LEVELS || node->pContenidos.size() <= MAX_TRI_NODE) {
        if (node->pContenidos.size() > maximo_triangulos_nodo) {
            maximo_triangulos_nodo = node->pContenidos.size();
        }
    }

    if (node->nivel >= MAX_LEVELS) {
        if (node->pContenidos.size() > MAX_TRI_NODE) {
            optimized = false; // No pudimos dividir más (alcanzado MAX_LEVELS)
            nodos_no_optimizados++;
        }
        return;
    }
    
    if (node->pContenidos.size() <= MAX_TRI_NODE) {
        return;
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
            // 2 rayos desde el centro
            vec3 centerBox = node->box.center();
            Vect3d center(centerBox.x, centerBox.y, centerBox.z);
            
            // Rayos
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
                // 3er rayo para desempatar
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
    std::cout << "Octree optimizado (<= " << MAX_TRI_NODE << " tri/nodo): " << (optimized ? "Si" : "No") 
              << " (" << nodos_no_optimizados << " veces superado limite)" << std::endl;
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

void Octree::colisiona(NodeOctree* nodoA, NodeOctree* nodoB, std::vector<std::pair<NodeOctree*, NodeOctree*>>& result_nodos)
{
    if (!nodoA->box.box_box(nodoB->box)) return;

    if (nodoA->esHoja() && nodoB->esHoja()) {
        result_nodos.push_back({nodoA, nodoB});
    } else if (nodoA->esHoja()) {
        for (int j = 0; j < 8; j++) {
            if (nodoB->hijos[j] != nullptr) {
                colisiona(nodoA, nodoB->hijos[j], result_nodos);
            }
        }
    } else if (nodoB->esHoja()) {
        for (int i = 0; i < 8; i++) {
            if (nodoA->hijos[i] != nullptr) {
                colisiona(nodoA->hijos[i], nodoB, result_nodos);
            }
        }
    } else {
        for (int i = 0; i < 8; i++) {
            if (nodoA->hijos[i] != nullptr) {
                for (int j = 0; j < 8; j++) {
                    if (nodoB->hijos[j] != nullptr) {
                        colisiona(nodoA->hijos[i], nodoB->hijos[j], result_nodos);
                    }
                }
            }
        }
    }
}

std::vector<Triangle3d *> Octree::collide(Octree &obj, std::vector<NodeOctree*>& out_intersected_nodes)
{
    std::vector<std::pair<NodeOctree*, NodeOctree*>> result_nodos;
    if (this->raiz != nullptr && obj.raiz != nullptr) {
        colisiona(this->raiz, obj.raiz, result_nodos);
    }

    std::vector<Triangle3d *> result_triangles;
    for (auto& par : result_nodos) {
        bool pushedNodeA = false;
        bool pushedNodeB = false;

        for (auto tri_j : par.first->pContenidos) {
            for (auto tri_k : par.second->pContenidos) {
                if (tri_j->tri_tri(*tri_k)) {
                    if (std::find(result_triangles.begin(), result_triangles.end(), tri_j) == result_triangles.end())
                        result_triangles.push_back(tri_j);
                    if (std::find(result_triangles.begin(), result_triangles.end(), tri_k) == result_triangles.end())
                        result_triangles.push_back(tri_k);

                    if (!pushedNodeA) {
                        out_intersected_nodes.push_back(par.first);
                        pushedNodeA = true;
                    }
                    if (!pushedNodeB) {
                        out_intersected_nodes.push_back(par.second);
                        pushedNodeB = true;
                    }
                }
            }
        }
    }
    return result_triangles;
}

void Octree::colisionaMat(NodeOctree* nodoA, NodeOctree* nodoB, const mat4& matA, const mat4& matB, AABB boxA, AABB boxB, std::vector<std::pair<NodeOctree*, NodeOctree*>>& result_nodos)
{
    if (!boxA.box_box(boxB)) return;

    if (nodoA->esHoja() && nodoB->esHoja()) {
        result_nodos.push_back({nodoA, nodoB});
    } else if (nodoA->esHoja()) {
        for (int j = 0; j < 8; j++) {
            if (nodoB->hijos[j] != nullptr)
                colisionaMat(nodoA, nodoB->hijos[j], matA, matB, boxA, nodoB->hijos[j]->box.dot(matB), result_nodos);
        }
    } else if (nodoB->esHoja()) {
        for (int i = 0; i < 8; i++) {
            if (nodoA->hijos[i] != nullptr)
                colisionaMat(nodoA->hijos[i], nodoB, matA, matB, nodoA->hijos[i]->box.dot(matA), boxB, result_nodos);
        }
    } else {
        AABB childBoxesB[8];
        bool hasB[8];
        for (int j = 0; j < 8; j++) {
            hasB[j] = (nodoB->hijos[j] != nullptr);
            if (hasB[j]) childBoxesB[j] = nodoB->hijos[j]->box.dot(matB);
        }

        for (int i = 0; i < 8; i++) {
            if (nodoA->hijos[i] != nullptr) {
                AABB childBoxA = nodoA->hijos[i]->box.dot(matA);
                for (int j = 0; j < 8; j++) {
                    if (hasB[j]) {
                        colisionaMat(nodoA->hijos[i], nodoB->hijos[j], matA, matB, childBoxA, childBoxesB[j], result_nodos);
                    }
                }
            }
        }
    }
}

std::vector<Triangle3d *> Octree::collideWithMatrices(Octree &obj, const mat4& matA, const mat4& matB,
    std::vector<NodeOctree*>& out_nodesA, std::vector<NodeOctree*>& out_nodesB, bool skipTriTest)
{
    std::vector<std::pair<NodeOctree*, NodeOctree*>> result_nodos;
    if (this->raiz != nullptr && obj.raiz != nullptr) {
        AABB rootA = this->raiz->box.dot(matA);
        AABB rootB = obj.raiz->box.dot(matB);
        colisionaMat(this->raiz, obj.raiz, matA, matB, rootA, rootB, result_nodos);
    }

    // Filtramos la duplicidad de nodos superpuestos usando conjuntos (Hash en O(1) en vez de O(n))
    std::unordered_set<NodeOctree*> setNodesA, setNodesB;
    for (auto& par : result_nodos) {
        if (setNodesA.insert(par.first).second)
            out_nodesA.push_back(par.first);
        if (setNodesB.insert(par.second).second)
            out_nodesB.push_back(par.second);
    }

    std::vector<Triangle3d *> result_triangles;
    if (skipTriTest) return result_triangles;

    // FASE FINAL: Testeamos intersecciones matemáticas triángulo-a-triángulo usando matrices globales
    std::unordered_set<Triangle3d*> addedTris;
    for (auto& par : result_nodos) {
        for (auto tri_j : par.first->pContenidos) {
            vec4 jAv = matA * vec4(tri_j->getA().getX(), tri_j->getA().getY(), tri_j->getA().getZ(), 1.0f);
            vec4 jBv = matA * vec4(tri_j->getB().getX(), tri_j->getB().getY(), tri_j->getB().getZ(), 1.0f);
            vec4 jCv = matA * vec4(tri_j->getC().getX(), tri_j->getC().getY(), tri_j->getC().getZ(), 1.0f);
            Vect3d vJA(jAv.x, jAv.y, jAv.z), vJB(jBv.x, jBv.y, jBv.z), vJC(jCv.x, jCv.y, jCv.z);
            Triangle3d matJ(vJA, vJB, vJC);

            for (auto tri_k : par.second->pContenidos) {
                vec4 kAv = matB * vec4(tri_k->getA().getX(), tri_k->getA().getY(), tri_k->getA().getZ(), 1.0f);
                vec4 kBv = matB * vec4(tri_k->getB().getX(), tri_k->getB().getY(), tri_k->getB().getZ(), 1.0f);
                vec4 kCv = matB * vec4(tri_k->getC().getX(), tri_k->getC().getY(), tri_k->getC().getZ(), 1.0f);
                Vect3d vKA(kAv.x, kAv.y, kAv.z), vKB(kBv.x, kBv.y, kBv.z), vKC(kCv.x, kCv.y, kCv.z);
                Triangle3d matK(vKA, vKB, vKC);

                if (matJ.tri_tri(matK)) {
                    if (addedTris.insert(tri_j).second)
                        result_triangles.push_back(tri_j);
                    if (addedTris.insert(tri_k).second)
                        result_triangles.push_back(tri_k);
                }
            }
        }
    }
    return result_triangles;
}

