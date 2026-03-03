#include "stdafx.h"
#include "DrawPointCloud3d.h"

// Public methods

AlgGeom::DrawPointCloud3d::DrawPointCloud3d(PointCloud3d& pointCloud) : Model3D(), _pointCloud(pointCloud)
{
    Component* component = new Component;

    for (int i = 0; i < (int)_pointCloud.size(); i++)
    {
        Vect3d p = _pointCloud.getPoint(i);
        component->_vertices.push_back(
            VAO::Vertex{ vec3((float)p.getX(), (float)p.getY(), (float)p.getZ()) }
        );
        component->_indices[VAO::IBO_POINT].push_back(i);
    }

    component->completeTopology();
    this->_components.push_back(std::unique_ptr<Component>(component));

    this->calculateAABB();
    this->buildVao(component);
}
