#include "stdafx.h"
#include "DrawTriangle3d.h"

// Public methods

AlgGeom::DrawTriangle3d::DrawTriangle3d(Triangle3d& triangle) : Model3D(), _triangle(triangle)
{
    Vect3d a = _triangle.getA();
    Vect3d b = _triangle.getB();
    Vect3d c = _triangle.getC();

    Vect3d ab = b.sub(a);
    Vect3d ac = c.sub(a);
    Vect3d norm = ab.xProduct(ac);
    double mod = norm.module();
    vec3 n(0.0f);
    if (mod > 0.0001)
    {
        n = glm::normalize(vec3((float)norm.getX(), (float)norm.getY(), (float)norm.getZ()));
    }

    Component* component = new Component;
    component->_vertices.insert(component->_vertices.end(), {
        VAO::Vertex{ vec3((float)a.getX(), (float)a.getY(), (float)a.getZ()), n },
        VAO::Vertex{ vec3((float)b.getX(), (float)b.getY(), (float)b.getZ()), n },
        VAO::Vertex{ vec3((float)c.getX(), (float)c.getY(), (float)c.getZ()), n }
    });

    component->_indices[VAO::IBO_TRIANGLE].insert(
        component->_indices[VAO::IBO_TRIANGLE].end(),
        { 0, 1, 2, RESTART_PRIMITIVE_INDEX }
    );

    component->completeTopology();
    this->_components.push_back(std::unique_ptr<Component>(component));

    this->calculateAABB();
    this->buildVao(component);
}
