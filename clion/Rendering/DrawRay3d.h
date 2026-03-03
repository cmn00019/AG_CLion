#pragma once

/* 
 *  File:   DrawRay3d.h
 *
 *  Created on 3 de marzo de 2026
 */

#include "Ray3d.h"
#include "Model3D.h"

namespace AlgGeom
{
    class DrawRay3d : public Model3D 
    {
    protected:
        Ray3d _ray;

    public:
        DrawRay3d(Ray3d& ray);
        DrawRay3d(const DrawRay3d& drawRay) = delete;
        ~DrawRay3d() override {};
    };
}
