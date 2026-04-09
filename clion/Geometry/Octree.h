#pragma once
#include <vector>
#include <string>
#include "NodeOctree.h"
#include "TriangleModel.h"

#define MAX_TRI_NODE 40
#define MAX_LEVELS 8

class Octree {
public:
    NodeOctree* raiz;
    TriangleModel* model;
    bool optimized;

    unsigned int rayos_indecision;
    unsigned int nivel_maximo;
    unsigned int maximo_triangulos_nodo;
    unsigned int nodos_no_optimizados;

    Octree(TriangleModel* bm_model, const std::string& objFile);
    ~Octree();

    void classify_color();
    NodeOctree* findLeaf(const Vect3d& p);
    void printStatistics() const;
    
    std::vector<Triangle3d *> collide(Octree &obj, std::vector<NodeOctree*>& out_intersected_nodes);
    std::vector<Triangle3d *> collideWithMatrices(Octree &obj, const mat4& matA, const mat4& matB,
        std::vector<NodeOctree*>& out_nodesA, std::vector<NodeOctree*>& out_nodesB, bool skipTriTest = false);

private:

    void buildNode(NodeOctree* node);
    void classifyNode(NodeOctree* node);
    NodeOctree* findLeafRec(NodeOctree* node, const Vect3d& p);
    int countRayIntersections(NodeOctree* node, Ray3d& ray, std::vector<Triangle3d*>& testedTriangles);
    void colisiona(NodeOctree* nodoA, NodeOctree* nodoB, std::vector<std::pair<NodeOctree*, NodeOctree*>>& result_nodos);
    void colisionaMat(NodeOctree* nodoA, NodeOctree* nodoB, const mat4& matA, const mat4& matB, AABB boxA, AABB boxB, std::vector<std::pair<NodeOctree*, NodeOctree*>>& result_nodos);

};
