#pragma once
#include "Model3D.h"
#include "Octree.h"

namespace AlgGeom {
    class DrawOctree : public Model3D {
    public:
        DrawOctree(Octree* octree);
        virtual ~DrawOctree();
    
    private:
        void addNodeToComponents(NodeOctree* node, Component* compWhite, Component* compGrey, Component* compBlack);
        void addAABBToComponent(const AABB& aabb, Component* comp);
    };
}
