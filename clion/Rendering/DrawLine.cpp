#include "stdafx.h"
#include "DrawLine.h"

// Public methods

AlgGeom::DrawLine::DrawLine (Line &line): Model3D(), _line(line)
{
	Point negEnd = line.getPoint(-1000);
	Point posEnd = line.getPoint(1000);

	Component* component = new Component;
    component->_vertices.insert(component->_vertices.end(), {
			VAO::Vertex { vec3(negEnd.getX(), negEnd.getY(), .0f) },
			VAO::Vertex { vec3(posEnd.getX(), posEnd.getY(), .0f) }
        });

    component->_indices[VAO::IBO_LINE].insert(component->_indices[VAO::IBO_LINE].end(), { 0, 1 });
    this->_components.push_back(std::unique_ptr<Component>(component));

    component->completeTopology();

    this->calculateAABB();
    this->buildVao(component);
}