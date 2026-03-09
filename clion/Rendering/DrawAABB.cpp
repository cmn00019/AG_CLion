#include "stdafx.h"
#include "DrawAABB.h"


AlgGeom::DrawAABB::DrawAABB(AABB &aabb): Model3D()
{
    Component* component = new Component;
    vec3 cMn = aabb.min(), cMx = aabb.max();

    vec3 corners[8] = {
        vec3(cMn.x,cMn.y,cMn.z), vec3(cMx.x,cMn.y,cMn.z), vec3(cMx.x,cMx.y,cMn.z), vec3(cMn.x,cMx.y,cMn.z),
        vec3(cMn.x,cMn.y,cMx.z), vec3(cMx.x,cMn.y,cMx.z), vec3(cMx.x,cMx.y,cMx.z), vec3(cMn.x,cMx.y,cMx.z)
    };

    for (int i = 0; i < 8; i++) 
    {
        component->_vertices.push_back(VAO::Vertex{ vec3(corners[i].x, corners[i].y, corners[i].z) });
    }

    int edges[12][2] = {{0,1},{1,2},{2,3},{3,0},{4,5},{5,6},{6,7},{7,4},{0,4},{1,5},{2,6},{3,7}};
    for (auto& e : edges) 
    {
        component->_indices[VAO::IBO_LINE].insert(component->_indices[VAO::IBO_LINE].end(), { (GLuint)e[0], (GLuint)e[1] });
    }

    this->_components.push_back(std::unique_ptr<Component>(component));

    this->calculateAABB();
    this->buildVao(component);
}

AlgGeom::DrawAABB::~DrawAABB()
{
}

