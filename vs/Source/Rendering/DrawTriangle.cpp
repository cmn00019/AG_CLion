#include "stdafx.h"
#include "DrawTriangle.h"

// Public methods

AlgGeom::DrawTriangle::DrawTriangle (Triangle &triangle): Model3D(), _triangle(triangle)
{  
    Component* component = new Component;
    component->_vertices.insert(component->_vertices.end(), {
            VAO::Vertex { vec3(_triangle.getA().getX(), _triangle.getA().getY(), .0f), vec3(.0f, .0f, 1.0f) },
            VAO::Vertex { vec3(_triangle.getB().getX(), _triangle.getB().getY(), .0f), vec3(.0f, .0f, 1.0f) },
            VAO::Vertex { vec3(_triangle.getC().getX(), _triangle.getC().getY(), .0f), vec3(.0f, .0f, 1.0f) }
        });
    component->_indices[VAO::IBO_TRIANGLE].insert(component->_indices[VAO::IBO_TRIANGLE].end(), { 0, 1, 2, RESTART_PRIMITIVE_INDEX });
    component->completeTopology();
    this->_components.push_back(std::unique_ptr<Component>(component));

    this->calculateAABB();
    this->buildVao(component);
}