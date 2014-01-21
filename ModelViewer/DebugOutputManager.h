#pragma once

#include <vector>
#include "vec3.h"
#include "mat3x3.h"

namespace SBGraphics
{
    class DebugOutput;
    class Entity;

    class DebugOutputManager
    {

    public:

        DebugOutputManager();
        ~DebugOutputManager();

        void update( float currentSimTime );
        void draw();

        void drawDebugLine( SBMath::vec3& startPosition, SBMath::vec3& endPosition );
        void drawDebugPoint( SBMath::vec3& position, SBMath::mat3x3& orientation );
        void drawDebugCube( SBMath::vec3& position, SBMath::vec3& axis, float size ); 
        void drawDebugHighlight( Entity& entity );

        void addDebugLine( SBMath::vec3& startPosition, SBMath::vec3& endPosition, float timeToDie = 1.0f );
        void addDebugPoint( SBMath::vec3& position, SBMath::mat3x3& orientation, float timeToDie = 1.0f );
        void addDebugCube( SBMath::vec3& position, SBMath::vec3& axis, float size, float timetoDie = 1.0f );
        void addDebugHighlight( Entity& entity, float timeToDie = 1.0f );

    private:

        float m_currentSimTime;
        std::vector< DebugOutput* > m_debugOutputs; 

    };
}