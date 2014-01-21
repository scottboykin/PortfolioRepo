#include <iostream>
#include "TechnologyBlueprint.h"
#include "XMLReader.h"
#include "StringParser.h"
#include "Technology.h"

using namespace SBUtil;

namespace SBGame
{
    TechnologyBlueprint::TechnologyBlueprint( SBUtil::XMLNode& node )
    {
        TiXmlElement& root = *node.ToElement();
        XMLReader reader;

        m_name = reader.getXMLAttributeAsString( root, "name", std::string("") );

        parseChild( root );
    }

    Technology* TechnologyBlueprint::createTechnologyFromBlueprint()
    {
        return new Technology( *this );
    }

    void TechnologyBlueprint::parseChild( TiXmlElement& element )
    {
        for( TiXmlElement* child = element.FirstChildElement(); child != 0; child = child->NextSiblingElement() )
        {
            parseChild( *child );
        }

        XMLReader reader;

        if( strcmp( element.Value(), "Cost" ) == 0 )
        {
            m_goldCost = reader.getXMLAttributeAsInt( element, "gold", 50 );
            m_stoneCost = reader.getXMLAttributeAsInt( element, "stone", 0 );
            m_timeToBuild = reader.getXMLAttributeAsFloat( element, "time", 10.0f );
        }

        else if( strcmp( element.Value(), "Unlocks" ) == 0 )
        {
            std::string unlocks = reader.getXMLAttributeAsString( element, "unlocks", std::string("") );

            StringParser unlocksParser( unlocks, std::string( ", " ) );

            while( !unlocksParser.stringFullyParsed() )
            {
                m_unlocks.push_back( unlocksParser.getNextToken() );
            }
            
        }
    }
}