#include "stdafx.h"
#include "DrawLine.h"

// Public methods

AlgGeom::DrawLine::DrawLine (Line &line): Model3D(), _line(line)
{
	Vect2d slope(line.getB().getX() - line.getA().getX(), line.getB().getY() - line.getA().getY());

	Component* component = new Component;
    component->_vertices.insert(component->_vertices.end(), {
			VAO::Vertex { vec3(line.getA().getX() - slope.getX() * 1000.0f, line.getA().getY() - slope.getY() * 1000.0f, .0f) },
			VAO::Vertex { vec3(line.getB().getX() + slope.getX() * 1000.0f, line.getB().getY() + slope.getY() * 1000.0f, .0f) }
        });

    component->_indices[VAO::IBO_LINE].insert(component->_indices[VAO::IBO_LINE].end(), { 0, 1 });
    this->_components.push_back(std::unique_ptr<Component>(component));

    component->completeTopology();

    this->calculateAABB();
    this->buildVao(component);
}