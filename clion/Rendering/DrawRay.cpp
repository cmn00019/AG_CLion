#include "stdafx.h"
#include "DrawRay.h"

// Public methods

AlgGeom::DrawRay::DrawRay (RayLine &ray): Model3D(), _ray(ray)
{
    Point pEnd = ray.getPoint(1000.0);

    Component* component = new Component;
    component->_vertices.insert(component->_vertices.end(), {
            VAO::Vertex { vec3((float)ray.getA().getX(), (float)ray.getA().getY(), .0f) },

            VAO::Vertex { vec3((float)pEnd.getX(), (float)pEnd.getY(), .0f) }
        });

    component->_indices[VAO::IBO_LINE].insert(component->_indices[VAO::IBO_LINE].end(), { 0, 1 });
    this->_components.push_back(std::unique_ptr<Component>(component));

    component->completeTopology();

    this->calculateAABB();
    this->buildVao(component);
}