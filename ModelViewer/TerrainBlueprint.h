#pragma once

#include <string>
#include "Color.h"
#include "XMLReader.h"

namespace SBGame
{
    class Terrain;

    class TerrainBlueprint
    {

    public:

        TerrainBlueprint( SBUtil::XMLNode& node );

        Terrain* createTerrainFromBlueprint();

        void parseChild( TiXmlElement& element );

        std::string& getName(){ return m_name; }

        bool getBlocksBuild(){ return m_blocksBuilding; }

        float getMovementModifer(){ return m_movementModifier; }

        SBGraphics::Color getColor(){ return m_color; }

    private:

        std::string m_name;

        bool m_blocksBuilding;

        float m_movementModifier;

        SBGraphics::Color m_color;

    };
}