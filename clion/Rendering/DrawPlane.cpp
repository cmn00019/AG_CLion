#include "stdafx.h"
#include "DrawPlane.h"

AlgGeom::DrawPlane::DrawPlane(Plane& plane) : Model3D(), _plane(plane) 
{
    const float scale = 5.0f;

    Vect3d normal = _plane.getNormal();
    double mod = normal.module();
    vec3 n(0.0f);
    if (mod > 0.0001) {
        n = glm::normalize(vec3((float)normal.getX(), (float)normal.getY(), (float)normal.getZ()));
    }

    struct PlaneAccess : public Plane {
        using Plane::_a;
        using Plane::_b;
        using Plane::_c;
    };
    PlaneAccess& pa = reinterpret_cast<PlaneAccess&>(_plane);
    Vect3d a = pa._a, b = pa._b, c = pa._c;

    // Calcular los 6 vectores de aristas extendidas:
    // P0 = a + scale * (b - a)  -> extensión de ab desde a
    // P1 = a + scale * (c - a)  -> extensión de ac desde a
    // P2 = b + scale * (c - b)  -> extensión de bc desde b
    // P3 = b + scale * (a - b)  -> extensión de ba desde b
    // P4 = c + scale * (a - c)  -> extensión de ca desde c
    // P5 = c + scale * (b - c)  -> extensión de cb desde c

    Vect3d ab = b.sub(a); Vect3d ac = c.sub(a);
    Vect3d bc = c.sub(b); Vect3d ba = a.sub(b);
    Vect3d ca = a.sub(c); Vect3d cb = b.sub(c);

    Vect3d sab = ab.scalarMul(scale); Vect3d p0 = a.add(sab);
    Vect3d sac = ac.scalarMul(scale); Vect3d p1 = a.add(sac);
    Vect3d sbc = bc.scalarMul(scale); Vect3d p2 = b.add(sbc);
    Vect3d sba = ba.scalarMul(scale); Vect3d p3 = b.add(sba);
    Vect3d sca = ca.scalarMul(scale); Vect3d p4 = c.add(sca);
    Vect3d scb = cb.scalarMul(scale); Vect3d p5 = c.add(scb);

    // Orden de los 6 puntos para formar un polígono convexo correcto:
    Vect3d pts[6] = { p0, p2, p1, p3, p5, p4 };

    Component* component = new Component;

    // Añadir los 6 vértices con la normal del plano
    for (int i = 0; i < 6; i++) {
        component->_vertices.push_back(
            VAO::Vertex { vec3((float)pts[i].getX(), (float)pts[i].getY(), (float)pts[i].getZ()), n }
        );
    }

    for (int i = 1; i < 5; i++) {
        component->_indices[VAO::IBO_TRIANGLE].insert(
            component->_indices[VAO::IBO_TRIANGLE].end(),
            { 0, (unsigned)(i), (unsigned)(i + 1), RESTART_PRIMITIVE_INDEX }
        );
    }

    // También añadir líneas de contorno
    for (int i = 0; i < 6; i++) {
        component->_indices[VAO::IBO_LINE].insert(
            component->_indices[VAO::IBO_LINE].end(),
            { (unsigned)i, (unsigned)((i + 1) % 6) }
        );
    }

    component->completeTopology();
    this->_components.push_back(std::unique_ptr<Component>(component));
    this->calculateAABB();
    this->buildVao(component);
}

AlgGeom::DrawPlane::~DrawPlane()
{
}
