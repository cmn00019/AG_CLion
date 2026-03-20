#include "stdafx.h"
#include "Octree.h"
#include "RandomUtilities.h"
#include "Ray3d.h"
#include <iostream>

Octree::Octree(TriangleModel* bm_model, const std::string& objFile)
    : model(bm_model), optimized(true)
{
    model->setOctree(this);
    
    // Calculate full AABB
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
    
    // Add all triangles to root
    auto faces = model->getFacesPtrs();
    for (auto tri : faces) {
        raiz->pContenidos.push_back(tri);
    }
    
    // Build the tree recursively
    buildNode(raiz);
}

Octree::~Octree()
{
    if (raiz) delete raiz;
}

void Octree::buildNode(NodeOctree* node)
{
    if (node->nivel >= MAX_LEVELS) {
        if (node->pContenidos.size() > MAX_TRI_NODE) {
            optimized = false; // We couldn't divide further
        }
        return;
    }
    
    if (node->pContenidos.size() <= MAX_TRI_NODE) {
        return; // Condition met, no subdivision needed
    }
    
    node->creaHijos();
    
    // Distribute children
    for (int i = 0; i < 8; i++) {
        NodeOctree* child = node->hijos[i];
        for (auto tri : node->pContenidos) {
            if (child->box.AABB_tri(*tri)) {
                child->pContenidos.push_back(tri);
            }
        }
        buildNode(child);
    }
    
    // Clear parent contents since they are pushed to leaves
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
            node->color = GREY; // Border of the model
        } else {
            // It has no triangles -> WHITE or BLACK
            // Shoot 2 rays from center
            vec3 centerBox = node->box.center();
            Vect3d center(centerBox.x, centerBox.y, centerBox.z);
            
            // Random rays instead of orthogonal to avoid grazing faces/edges 
            vec3 v1 = RandomUtilities::getUniformRandomInUnitSphere();
            vec3 v2 = RandomUtilities::getUniformRandomInUnitSphere();
            Vect3d dir1(v1.x, v1.y, v1.z);
            Vect3d dir2(v2.x, v2.y, v2.z);
            
            Vect3d dest1(center.getX()+dir1.getX(), center.getY()+dir1.getY(), center.getZ()+dir1.getZ());
            Vect3d dest2(center.getX()+dir2.getX(), center.getY()+dir2.getY(), center.getZ()+dir2.getZ());
            
            Ray3d ray1(center, dest1);
            Ray3d ray2(center, dest2);
            
            bool odd1 = model->hasOddIntersections(ray1);
            bool odd2 = model->hasOddIntersections(ray2);
            
            if (odd1 && odd2) {
                node->color = BLACK;
            } else if (!odd1 && !odd2) {
                node->color = WHITE;
            } else {
                // Murphy's law -> 3rd ray to break tie
                vec3 v3 = RandomUtilities::getUniformRandomInUnitSphere();
                Vect3d dir3(v3.x, v3.y, v3.z);
                Vect3d dest3(center.getX()+dir3.getX(), center.getY()+dir3.getY(), center.getZ()+dir3.getZ());
                Ray3d ray3(center, dest3);
                bool odd3 = model->hasOddIntersections(ray3);
                
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
