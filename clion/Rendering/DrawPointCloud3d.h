#pragma once

/* 
 *  File:   DrawPointCloud3d.h
 *
 *  Created on 3 de marzo de 2026
 */

#include "PointCloud3d.h"
#include "Model3D.h"

namespace AlgGeom
{
    class DrawPointCloud3d : public Model3D 
    {
    protected:
        PointCloud3d _pointCloud;

    public:
        DrawPointCloud3d(PointCloud3d& pointCloud);
        DrawPointCloud3d(const DrawPointCloud3d& drawPointCloud) = delete;
        ~DrawPointCloud3d() override {};
    };
}
