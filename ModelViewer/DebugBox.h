#pragma once

#include <vector>

#include "DebugOutput.h"
#include "Vertex.h"

namespace SBGraphics
{
    class DebugBox : public DebugOutput
    {

    public:

        DebugBox( SBMath::vec3& position, SBMath::vec3& axis, float size, float timetoDie = 0.0f );

        virtual void draw();

    private:

        void setupVertices();

        std::vector< Vertex > m_vertices;

        float m_size;

        SBMath::vec3 m_axis;
        SBMath::vec3 m_position;
    };
}