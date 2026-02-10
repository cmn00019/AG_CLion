#include "stdafx.h"
#include "DrawCircle.h"

// Public methods

AlgGeom::DrawCircle::DrawCircle(Circle& circle, uint32_t numSubdivisions): Model3D(), _circle(circle), _numSubdivisions(numSubdivisions)
{
    Component* component = new Component;

    Polygon polygon = _circle.getPointsCircle();
    size_t numVertices = polygon.getNumVertices();

    for (unsigned vertexIdx = 0; vertexIdx < polygon.getNumVertices(); vertexIdx++) {

        Point point = polygon.getVertexAt(vertexIdx).getPoint();
        component->_vertices.push_back(VAO::Vertex{ vec3(point.getX(), point.getY(), .0f), vec3(.0f, .0f, 1.0f) });
    }
    component->_vertices.push_back(VAO::Vertex{ vec3(.0f, .0f, .0f) });

    for (unsigned vertexIdx = 0; vertexIdx < polygon.getNumVertices(); vertexIdx++) {

        component->_indices[VAO::IBO_TRIANGLE].insert(component->_indices[VAO::IBO_TRIANGLE].end(), 
            { static_cast<unsigned>(component->_vertices.size()) - 1, vertexIdx, (vertexIdx + 1) % numVertices, RESTART_PRIMITIVE_INDEX });
        component->_indices[VAO::IBO_LINE].insert(component->_indices[VAO::IBO_LINE].end(), { vertexIdx, (vertexIdx + 1) % numVertices, RESTART_PRIMITIVE_INDEX });
    }

    component->completeTopology();
    this->_components.push_back(std::unique_ptr<Component>(component));

    this->calculateAABB();
    this->buildVao(component);
}
