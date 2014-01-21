#pragma once

#include "DebugOutput.h"
#include "vec3.h"
#include "mat3x3.h"
#include "Vertex.h"

namespace SBGraphics
{
    class DebugPoint : public DebugOutput
    {

    public:

        DebugPoint( SBMath::vec3& position, SBMath::mat3x3& orientation, float timeToDie = 0.0f );

        virtual void draw();

        void setupAxes();

    private:

        SBMath::vec3 m_position;
        SBMath::mat3x3 m_orientation;

        SBGraphics::Vertex m_solidAxes[6];
        SBGraphics::Vertex m_translucentAxes[6];

    };
}