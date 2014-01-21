#pragma once

#include <string>
#include "TechnologyBlueprint.h"

namespace SBGame
{
    class Technology
    {

    public:

        Technology( TechnologyBlueprint& blueprint );

        std::string& getName(){ return *m_name; }

        std::vector< std::string >& getUnlocks(){ return *m_unlocks; }

        int getGoldCost(){ return m_goldCost; }
        int getStoneCost(){ return m_stoneCost; }

        float getTimeToBuild(){ return m_timeToBuild; }

    private:

        std::string* m_name;
        std::vector< std::string >* m_unlocks;

        int m_goldCost;
        int m_stoneCost;

        float m_timeToBuild;

    };
}