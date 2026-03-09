#include "stdafx.h"
#include "DrawLine3d.h"

// Public methods

AlgGeom::DrawLine3d::DrawLine3d(Line3d& line) : Model3D(), _line(line)
{
    // Simulamos la recta infinita con un "t" alto
    Vect3d pStart = line.getPoint(-1000.0);
    Vect3d pEnd   = line.getPoint(1000.0);

    Component* component = new Component;
    component->_vertices.insert(component->_vertices.end(), {
        VAO::Vertex{ vec3((float)pStart.getX(), (float)pStart.getY(), (float)pStart.getZ()) },
        VAO::Vertex{ vec3((float)pEnd.getX(),   (float)pEnd.getY(),   (float)pEnd.getZ()) }
    });

    component->_indices[VAO::IBO_LINE].insert(component->_indices[VAO::IBO_LINE].end(), { 0, 1 });
    this->_components.push_back(std::unique_ptr<Component>(component));

    component->completeTopology();

    this->calculateAABB();
    this->buildVao(component);
}
