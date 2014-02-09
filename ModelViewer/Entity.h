#pragma once

#include <vector>

#include "vec3.h"
#include "Vertex.h"

namespace SBGraphics
{
    class Entity
    {

    public:

        Entity();

        virtual void update( float dt );
        virtual void draw();

        SBMath::vec3& getPosition(){ return m_position; }
        void setPosition( SBMath::vec3& position ){ m_position = position; }

        SBMath::vec3& getOrientation(){ return m_orientation; }

        std::vector< Vertex >& getVertices(){ return m_vertices; }

    protected:

        void setupVertices();

        SBMath::vec3 m_position;
        SBMath::vec3 m_orientation;

        std::vector< Vertex > m_vertices;
        std::vector< unsigned int > m_indices;

    };
}