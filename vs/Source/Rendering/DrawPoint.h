#pragma once

/* 
 *  File:   DrawPoint.h
 *  Author: lidia
 *
 *  Created on 14 de enero de 2021, 15:02
 */

#include "Point.h"
#include "Model3D.h"

namespace AlgGeom
{
    class DrawPoint : public Model3D 
    {
    protected:
        Point _point;

    public:
        DrawPoint(Point& p);
        DrawPoint(const DrawPoint& drawPoint) = delete;
        ~DrawPoint() override {};
    };
}

