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

    Octree(TriangleModel* bm_model, const std::string& objFile);
    ~Octree();

    void classify_color();
    NodeOctree* findLeaf(const Vect3d& p);
    void printStatistics() const;

private:
    void buildNode(NodeOctree* node);
    void classifyNode(NodeOctree* node);
    NodeOctree* findLeafRec(NodeOctree* node, const Vect3d& p);
    int countRayIntersections(NodeOctree* node, Ray3d& ray, std::vector<Triangle3d*>& testedTriangles);
};
