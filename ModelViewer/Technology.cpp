#include <iostream>
#include "Technology.h"
#include "XMLReader.h"

using namespace SBUtil;

namespace SBGame
{
    Technology::Technology( TechnologyBlueprint& blueprint  )
        :   m_name( &blueprint.getName() )
        ,   m_unlocks( &blueprint.getUnlocks() )
        ,   m_goldCost( blueprint.getGoldCost() )
        ,   m_stoneCost( blueprint.getStoneCost() )
        ,   m_timeToBuild( blueprint.getTimeToBuild() )
    {}
}