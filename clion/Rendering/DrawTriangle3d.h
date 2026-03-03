#pragma once

/* 
 *  File:   DrawTriangle3d.h
 *
 *  Created on 3 de marzo de 2026
 */

#include "Triangle3d.h"
#include "Model3D.h"

namespace AlgGeom
{
    class DrawTriangle3d : public Model3D 
    {
    protected:
        Triangle3d _triangle;

    public:
        DrawTriangle3d(Triangle3d& triangle);
        DrawTriangle3d(const DrawTriangle3d& drawTriangle) = delete;
        ~DrawTriangle3d() override {};
    };
}
