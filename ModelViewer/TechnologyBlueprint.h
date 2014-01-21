#pragma once

#include <string>
#include <vector>
#include "XMLReader.h"

namespace SBGame
{
    class Technology;

    class TechnologyBlueprint
    {

    public:

        TechnologyBlueprint( SBUtil::XMLNode& node );

        Technology* createTechnologyFromBlueprint();

        void TechnologyBlueprint::parseChild( TiXmlElement& element );

        std::string& getName(){ return m_name; }

        std::vector< std::string >& getUnlocks(){ return m_unlocks; }

        int getGoldCost(){ return m_goldCost; }
        int getStoneCost(){ return m_stoneCost; }

        float getTimeToBuild(){ return m_timeToBuild; }

    private:

        std::string m_name;
        std::vector< std::string > m_unlocks;

        int m_goldCost;
        int m_stoneCost;

        float m_timeToBuild;

    };
}