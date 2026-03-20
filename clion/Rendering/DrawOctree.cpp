#include "stdafx.h"
#include "DrawOctree.h"

using namespace AlgGeom;

DrawOctree::DrawOctree(Octree* octree) : Model3D() {
    Component* compWhite = new Component;
    compWhite->_material._lineColor = vec3(1.0f, 1.0f, 1.0f); // White
    
    Component* compGrey = new Component;
    compGrey->_material._lineColor = vec3(0.5f, 0.5f, 0.5f); // Grey
    
    Component* compBlack = new Component;
    compBlack->_material._lineColor = vec3(0.0f, 0.0f, 0.0f); // Black

    if (octree && octree->raiz) {
        addNodeToComponents(octree->raiz, compWhite, compGrey, compBlack);
    }
    
    if (compWhite->_vertices.size() > 0) {
        this->_components.push_back(std::unique_ptr<Component>(compWhite));
        this->buildVao(compWhite);
    } else {
        delete compWhite;
    }
    
    if (compGrey->_vertices.size() > 0) {
        this->_components.push_back(std::unique_ptr<Component>(compGrey));
        this->buildVao(compGrey);
    } else {
        delete compGrey;
    }
    
    if (compBlack->_vertices.size() > 0) {
        this->_components.push_back(std::unique_ptr<Component>(compBlack));
        this->buildVao(compBlack);
    } else {
        delete compBlack;
    }
    
    this->calculateAABB();
}

DrawOctree::~DrawOctree() {}

void DrawOctree::addNodeToComponents(NodeOctree* node, Component* compWhite, Component* compGrey, Component* compBlack) {
    if (!node) return;
    
    if (node->esHoja()) {
        if (node->color == WHITE) addAABBToComponent(node->box, compWhite);
        else if (node->color == GREY) addAABBToComponent(node->box, compGrey);
        else if (node->color == BLACK) addAABBToComponent(node->box, compBlack);
    } else {
        for (int i = 0; i < 8; i++) {
            addNodeToComponents(node->hijos[i], compWhite, compGrey, compBlack);
        }
    }
}

void DrawOctree::addAABBToComponent(const AABB& aabb, Component* comp) {
    vec3 cMn = aabb.min(), cMx = aabb.max();
    vec3 corners[8] = {
        vec3(cMn.x,cMn.y,cMn.z), vec3(cMx.x,cMn.y,cMn.z), vec3(cMx.x,cMx.y,cMn.z), vec3(cMn.x,cMx.y,cMn.z),
        vec3(cMn.x,cMn.y,cMx.z), vec3(cMx.x,cMn.y,cMx.z), vec3(cMx.x,cMx.y,cMx.z), vec3(cMn.x,cMx.y,cMx.z)
    };
    
    GLuint startIdx = comp->_vertices.size();
    for (int i = 0; i < 8; i++) {
        comp->_vertices.push_back(VAO::Vertex{ corners[i] });
    }
    
    int edges[12][2] = {{0,1},{1,2},{2,3},{3,0},{4,5},{5,6},{6,7},{7,4},{0,4},{1,5},{2,6},{3,7}};
    for (auto& e : edges) {
        comp->_indices[VAO::IBO_LINE].insert(comp->_indices[VAO::IBO_LINE].end(), { startIdx + e[0], startIdx + e[1] });
    }
}
