#include "stdafx.h"
#include "DrawPlane.h"

AlgGeom::DrawPlane::DrawPlane(Plane& plane) : Model3D(), _plane(plane) 
{
    // Obtener los tres puntos del plano (accedemos via copia del plano almacenado)
    // Usamos la técnica de alargar los vectores de las aristas: ab, ac, bc, ba, ca, cb
    // para obtener 6 puntos que forman un polígono 3D representando el plano.

    // Acceder a los 3 puntos del plano: _a, _b, _c (protegidos en Plane, DrawPlane hereda indirectamente)
    // Como Plane almacena _a, _b, _c como protegidos, y DrawPlane tiene _plane como miembro,
    // necesitamos extraer los puntos. Usamos getNormal y los puntos originales.
    // Los puntos se obtienen del constructor: Plane(a, b, c, true) guarda _a=a, _b=b, _c=c.

    // Factor de escala para alargar los vectores
    const float scale = 5.0f;

    // Obtenemos la normal del plano para usarla en los vértices
    Vect3d normal = _plane.getNormal();
    double mod = normal.module();
    vec3 n(0.0f);
    if (mod > 0.0001) {
        n = glm::normalize(vec3((float)normal.getX(), (float)normal.getY(), (float)normal.getZ()));
    }

    // Extraemos los tres puntos del plano a través de la interfaz pública.
    // Plane almacena _a, _b, _c. Usamos getA/getB/getC para las componentes de la ecuación,
    // pero necesitamos los puntos directamente. Como DrawPlane tiene _plane como miembro
    // y Plane::_a, _b, _c son protected, creamos una subclase helper o usamos otro enfoque.
    // Dado que Plane no expone directamente _a, _b, _c, construimos los puntos
    // usando la normal y el término D para posicionar el plano correctamente.

    // Enfoque alternativo: Construir 6 puntos directamente desde los vectores de aristas.
    // Necesitamos acceder a _a, _b, _c. Como son protected, DrawPlane.h no hereda de Plane,
    // así que usamos un truco: creamos un accessor local.
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
    // El orden debe ser: P0, P2, P1, P3, P5, P4
    // (alternando puntos de extensiones opuestas para mantener convexidad)
    Vect3d pts[6] = { p0, p2, p1, p3, p5, p4 };

    Component* component = new Component;

    // Añadir los 6 vértices con la normal del plano
    for (int i = 0; i < 6; i++) {
        component->_vertices.push_back(
            VAO::Vertex { vec3((float)pts[i].getX(), (float)pts[i].getY(), (float)pts[i].getZ()), n }
        );
    }

    // Triangular como fan desde el vértice 0: (0,1,2), (0,2,3), (0,3,4), (0,4,5)
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
