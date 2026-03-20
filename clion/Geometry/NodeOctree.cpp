#include "stdafx.h"
#include "NodeOctree.h"

NodeOctree::NodeOctree(int nnivel, const vec3& minBox, const vec3& maxBox, Octree* o)
    : color(WHITE), box(minBox, maxBox), nivel(nnivel), oct(o)
{
    for (int i = 0; i < 8; i++) {
        hijos[i] = nullptr;
    }
}

NodeOctree::~NodeOctree()
{
    for (int i = 0; i < 8; i++) {
        if (hijos[i] != nullptr) {
            delete hijos[i];
        }
    }
}

void NodeOctree::creaHijos()
{
    vec3 minB = box.min();
    vec3 maxB = box.max();
    vec3 med = box.center();

    hijos[0] = new NodeOctree(nivel + 1, vec3(minB.x, minB.y, minB.z), vec3(med.x, med.y, med.z), oct);
    hijos[1] = new NodeOctree(nivel + 1, vec3(med.x, minB.y, minB.z), vec3(maxB.x, med.y, med.z), oct);
    hijos[2] = new NodeOctree(nivel + 1, vec3(minB.x, med.y, minB.z), vec3(med.x, maxB.y, med.z), oct);
    hijos[3] = new NodeOctree(nivel + 1, vec3(med.x, med.y, minB.z), vec3(maxB.x, maxB.y, med.z), oct);
    hijos[4] = new NodeOctree(nivel + 1, vec3(minB.x, minB.y, med.z), vec3(med.x, med.y, maxB.z), oct);
    hijos[5] = new NodeOctree(nivel + 1, vec3(med.x, minB.y, med.z), vec3(maxB.x, med.y, maxB.z), oct);
    hijos[6] = new NodeOctree(nivel + 1, vec3(minB.x, med.y, med.z), vec3(med.x, maxB.y, maxB.z), oct);
    hijos[7] = new NodeOctree(nivel + 1, vec3(med.x, med.y, med.z), vec3(maxB.x, maxB.y, maxB.z), oct);
}

bool NodeOctree::esHoja() const
{
    return hijos[0] == nullptr;
}
