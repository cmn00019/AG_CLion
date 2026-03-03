#include "stdafx.h"
#include "DrawRay3d.h"

// Public methods

AlgGeom::DrawRay3d::DrawRay3d(Ray3d& ray) : Model3D(), _ray(ray)
{
    Vect3d orig = ray.getOrigin();
    Vect3d pEnd = ray.getPoint(1000.0);

    Component* component = new Component;
    component->_vertices.insert(component->_vertices.end(), {
        VAO::Vertex{ vec3((float)orig.getX(), (float)orig.getY(), (float)orig.getZ()) },
        VAO::Vertex{ vec3((float)pEnd.getX(), (float)pEnd.getY(), (float)pEnd.getZ()) }
    });

    component->_indices[VAO::IBO_LINE].insert(component->_indices[VAO::IBO_LINE].end(), { 0, 1 });
    this->_components.push_back(std::unique_ptr<Component>(component));

    component->completeTopology();

    this->calculateAABB();
    this->buildVao(component);
}
