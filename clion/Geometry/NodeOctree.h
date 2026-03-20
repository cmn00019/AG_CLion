#pragma once
#include <vector>
#include "AABB.h"

class Triangle3d;
class Octree;

enum TypeColorNode { WHITE, BLACK, GREY };

class NodeOctree {
public:
    TypeColorNode color;
    AABB box;
    NodeOctree* hijos[8];
    std::vector<Triangle3d*> pContenidos;
    int nivel;
    Octree* oct;

    NodeOctree(int nnivel, const vec3& minBox, const vec3& maxBox, Octree* o);
    ~NodeOctree();

    void creaHijos();
    bool esHoja() const;
};
