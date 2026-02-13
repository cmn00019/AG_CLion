#include "stdafx.h"
#include "DrawLine.h"

// Public methods

AlgGeom::DrawLine::DrawLine (Line &line): Model3D(), _line(line)
{
	//Usamos una t alta para simular la recta infinita
	Point pStart = line.getPoint(-1000.0);
	Point pEnd   = line.getPoint(1000.0);

	Component* component = new Component;

	// Insertamos los vértices calculados
	component->_vertices.insert(component->_vertices.end(), {
		  VAO::Vertex { vec3((float)pStart.getX(), (float)pStart.getY(), .0f) },
		  VAO::Vertex { vec3((float)pEnd.getX(),   (float)pEnd.getY(),   .0f) }
		});

	component->_indices[VAO::IBO_LINE].insert(component->_indices[VAO::IBO_LINE].end(), { 0, 1 });
	this->_components.push_back(std::unique_ptr<Component>(component));

	component->completeTopology();

	this->calculateAABB();
	this->buildVao(component);
}