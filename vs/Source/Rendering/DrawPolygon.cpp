#include "stdafx.h"
#include "DrawPolygon.h"

// Public methods

AlgGeom::DrawPolygon::DrawPolygon (Polygon &polygon): Model3D(), _polygon(polygon) 
{   
    size_t numVertices = _polygon.getNumVertices();
    Component* component = new Component;

    for (unsigned vertexIdx = 0; vertexIdx < _polygon.getNumVertices(); vertexIdx++) {

        Point point = _polygon.getVertexAt(vertexIdx).getPoint();

        component->_vertices.push_back(VAO::Vertex{ vec3(point.getX(), point.getY(), .0f) });
        component->_indices[VAO::IBO_LINE].insert(component->_indices[VAO::IBO_LINE].end(), { vertexIdx, (vertexIdx + 1) % numVertices, RESTART_PRIMITIVE_INDEX });
    }

    this->_components.push_back(std::unique_ptr<Component>(component));

    this->calculateAABB();
    this->buildVao(component);
}