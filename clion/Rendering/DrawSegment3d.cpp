#include "stdafx.h"
#include "DrawSegment3d.h"

// Public methods

AlgGeom::DrawSegment3d::DrawSegment3d(Segment3d& segment) : Model3D(), _segment(segment)
{
    Vect3d orig = segment.getOrigin();
    Vect3d dest = segment.getDestination();

    Component* component = new Component;
    component->_vertices.insert(component->_vertices.end(), {
        VAO::Vertex{ vec3((float)orig.getX(), (float)orig.getY(), (float)orig.getZ()) },
        VAO::Vertex{ vec3((float)dest.getX(), (float)dest.getY(), (float)dest.getZ()) }
    });

    component->_indices[VAO::IBO_LINE].insert(component->_indices[VAO::IBO_LINE].end(), { 0, 1 });
    this->_components.push_back(std::unique_ptr<Component>(component));

    component->completeTopology();

    this->calculateAABB();
    this->buildVao(component);
}
