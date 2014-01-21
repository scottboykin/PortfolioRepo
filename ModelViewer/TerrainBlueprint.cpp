#include <iostream>
#include "TerrainBlueprint.h"
#include "XMLReader.h"
#include "Terrain.h"

using namespace SBUtil;
using namespace SBGraphics;

namespace SBGame
{
    TerrainBlueprint::TerrainBlueprint( SBUtil::XMLNode& node )
    {
        TiXmlElement& root = *node.ToElement();
        XMLReader reader;

        m_name = reader.getXMLAttributeAsString( root, "name", std::string("") );
        m_blocksBuilding = reader.getXMLAttributeAsBool( root, "blockBuilding", false );
        m_movementModifier = reader.getXMLAttributeAsFloat( root, "movementModifier", 1.0f );
        m_color = reader.getXMLAttributeAsColor( root, "color", Color() );

    }

    Terrain* TerrainBlueprint::createTerrainFromBlueprint()
    {
        return new Terrain( *this );
    }

    void TerrainBlueprint::parseChild( TiXmlElement& element )
    {
        for( TiXmlElement* child = element.FirstChildElement(); child != 0; child = child->NextSiblingElement() )
        {
            parseChild( *child );
        }
    }
}