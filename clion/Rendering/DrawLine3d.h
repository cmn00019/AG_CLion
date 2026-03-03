#pragma once

/* 
 *  File:   DrawLine3d.h
 *
 *  Created on 3 de marzo de 2026
 */

#include "Line3d.h"
#include "Model3D.h"

namespace AlgGeom
{
    class DrawLine3d : public Model3D 
    {
    protected:
        Line3d _line;

    public:
        DrawLine3d(Line3d& line);
        DrawLine3d(const DrawLine3d& drawLine) = delete;
        ~DrawLine3d() override {};
    };
}
