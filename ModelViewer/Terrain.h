#pragma once

#include <string>
#include "Color.h"
#include "TerrainBlueprint.h"

namespace SBGame
{
    class Terrain
    {

    public:

        Terrain();
        Terrain( TerrainBlueprint& blueprint );

        void render( int x, int y );

        std::string* getName(){ return m_name; }

        bool getBuildable(){ return m_blocksBuilding; }

        float getMovementModifer(){ return m_movementModifier; }

    private:

        std::string* m_name;

        bool m_blocksBuilding;

        float m_movementModifier;

        SBGraphics::Color m_color;

    };
}