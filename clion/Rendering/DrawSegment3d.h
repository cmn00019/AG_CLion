#pragma once

/* 
 *  File:   DrawSegment3d.h
 *
 *  Created on 3 de marzo de 2026
 */

#include "Segment3d.h"
#include "Model3D.h"

namespace AlgGeom
{
    class DrawSegment3d : public Model3D 
    {
    protected:
        Segment3d _segment;

    public:
        DrawSegment3d(Segment3d& segment);
        DrawSegment3d(const DrawSegment3d& drawSegment) = delete;
        ~DrawSegment3d() override {};
    };
}
