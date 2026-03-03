#include "stdafx.h"
#include "DrawPoint3d.h"

// Public methods

AlgGeom::DrawPoint3d::DrawPoint3d(Vect3d& point) : Model3D(), _point(point)
{
    Component* component = new Component;
    component->_vertices.push_back(
        VAO::Vertex{ vec3((float)_point.getX(), (float)_point.getY(), (float)_point.getZ()) }
    );
    component->_indices[VAO::IBO_POINT].push_back(0);
    component->completeTopology();
    this->_components.push_back(std::unique_ptr<Component>(component));

    this->calculateAABB();
    this->buildVao(component);
}
