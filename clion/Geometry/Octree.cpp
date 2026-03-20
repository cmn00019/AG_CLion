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
            
            // Random rays or orthogonal ones 
            Vect3d dir1(1.0, 0.0, 0.0);
            Vect3d dir2(0.0, 1.0, 0.0);
            
            Vect3d dest1(center.getX()+dir1.getX(), center.getY()+dir1.getY(), center.getZ()+dir1.getZ());
            Vect3d dest2(center.getX()+dir2.getX(), center.getY()+dir2.getY(), center.getZ()+dir2.getZ());
            
            Ray3d ray1(center, dest1);
            Ray3d ray2(center, dest2);
            
            auto hits1 = model->rayTravesal(ray1);
            auto hits2 = model->rayTravesal(ray2);
            
            bool odd1 = (hits1.size() % 2) != 0;
            bool odd2 = (hits2.size() % 2) != 0;
            
            if (odd1 && odd2) {
                node->color = BLACK;
            } else if (!odd1 && !odd2) {
                node->color = WHITE;
            } else {
                // Murphy's law -> 3rd ray to break tie
                Vect3d dir3(0.0, 0.0, 1.0);
                Vect3d dest3(center.getX()+dir3.getX(), center.getY()+dir3.getY(), center.getZ()+dir3.getZ());
                Ray3d ray3(center, dest3);
                auto hits3 = model->rayTravesal(ray3);
                bool odd3 = (hits3.size() % 2) != 0;
                
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
    
    vec3 minB = node->box.min();
    vec3 maxB = node->box.max();
    vec3 med = node->box.center();
    
    Vect3d p_copy = p;
    double coorX = p_copy.getX(), coorY = p_copy.getY(), coorZ = p_copy.getZ();
    double medX = med.x, medY = med.y, medZ = med.z;
    
    if(coorY >= medY){  
         if(coorX >= medX){ 
              if(coorZ >= medZ){ return findLeafRec(node->hijos[7], p); }  
              else { return findLeafRec(node->hijos[5], p); }              
          } else { 
              if(coorZ >= medZ){ return findLeafRec(node->hijos[6], p); }  
              else { return findLeafRec(node->hijos[4], p); }              
         }
   } else { 
          if(coorX >= medX){ 
             if(coorZ >= medZ){ return findLeafRec(node->hijos[3], p); }  
             else { return findLeafRec(node->hijos[1], p); }              
          } else { 
             if(coorZ >= medZ){ return findLeafRec(node->hijos[2], p); } 
             else { return findLeafRec(node->hijos[0], p); }              
          }
   }
   return nullptr;
}

NodeOctree* Octree::findLeaf(const Vect3d& p)
{
    return findLeafRec(raiz, p);
}
