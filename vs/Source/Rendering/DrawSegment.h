#pragma once

/* 
 *  File:   DrawSegment.h
 *  Author: lidia
 *
 *  Created on 19 de enero de 2021, 13:10
 */

#include "SegmentLine.h"
#include "Model3D.h"

namespace AlgGeom
{
    class DrawSegment : public Model3D {
    protected:
        SegmentLine _segment;

    public:
        DrawSegment(SegmentLine& t);
        DrawSegment(const DrawSegment& drawSegment) = delete;
        ~DrawSegment() override {};
    };
}