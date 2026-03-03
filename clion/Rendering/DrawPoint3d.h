#pragma once

/* 
 *  File:   DrawPoint3d.h
 *
 *  Created on 3 de marzo de 2026
 */

#include "Vect3d.h"
#include "Model3D.h"

namespace AlgGeom
{
    class DrawPoint3d : public Model3D 
    {
    protected:
        Vect3d _point;

    public:
        DrawPoint3d(Vect3d& p);
        DrawPoint3d(const DrawPoint3d& drawPoint) = delete;
        ~DrawPoint3d() override {};
    };
}
