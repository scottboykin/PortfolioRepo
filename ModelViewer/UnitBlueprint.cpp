#include <iostream>
#include "UnitBlueprint.h"
#include "XMLReader.h"
#include "Unit.h"

using namespace SBUtil;
using namespace SBMath;

namespace SBGame
{
    UnitBlueprint::UnitBlueprint( SBUtil::XMLNode& node )
    {
        TiXmlElement& root = *node.ToElement();
        XMLReader reader;

        m_name = reader.getXMLAttributeAsString( root, "name", std::string("") );

        parseChild( root );
    }

    Unit* UnitBlueprint::createUnitFromBlueprint( char teamNum, vec2 position )
    {
        return new Unit( *this, teamNum, position );
    }

    void UnitBlueprint::parseChild( TiXmlElement& element )
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

        else if( strcmp( element.Value(), "Combat" ) == 0 )
        {
            m_maxHealth = reader.getXMLAttributeAsFloat( element, "maxHealth", 10.0f );
            m_damage = reader.getXMLAttributeAsFloat( element, "damage", 0.0f );
            m_armor = reader.getXMLAttributeAsFloat( element, "armor", 0.0f );
            m_moveSpeed = reader.getXMLAttributeAsFloat( element, "moveSpeed", 0.0f );

            std::string damageTypeString = reader.getXMLAttributeAsString( element, "damageType", std::string("melee") );
            
            switch( damageTypeString[0] )
            {
                case 'm': m_damageType = MELEE; break;
                case 'r': m_damageType = RANGE; break;
                case 'h': m_damageType = EXPLOSION; break;
            }

            std::string armorTypeString = reader.getXMLAttributeAsString( element, "armorType", std::string("light") );

            switch( armorTypeString[0] )
            {
                case 'l': m_armorType = LIGHT; break;
                case 'm': m_armorType = MEDIUM; break;
                case 'h': m_armorType = HEAVY; break;
                case 's': m_armorType = STRUCTURE; break;
            }
        }

        else if( strcmp( element.Value(), "Abilities" ) == 0 )
        {
            m_abilitiesNames[0] = reader.getXMLAttributeAsString( element, "ability0", std::string("") );
            m_abilitiesNames[1] = reader.getXMLAttributeAsString( element, "ability1", std::string("") );
            m_abilitiesNames[2] = reader.getXMLAttributeAsString( element, "ability2", std::string("") );
            m_abilitiesNames[3] = reader.getXMLAttributeAsString( element, "ability3", std::string("") );
        }

    }
}