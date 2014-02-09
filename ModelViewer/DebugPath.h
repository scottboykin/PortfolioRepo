#pragma once

#include "DebugOutput.h"
#include "vec3.h"

namespace SBGraphics
{
    class DebugPath : public DebugOutput
    {

    public:

        DebugPath( SBMath::vec3& startPosition, SBMath::vec3& endPosition, float timeToDie = 0.0f );

        virtual void draw();

    private:

        void setupVertices();

        SBMath::vec3 m_startPos;
        SBMath::vec3 m_endPos;

        float m_vertices[28];

    };
}