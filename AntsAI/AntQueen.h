#pragma once

#include "Ant.h"
#include "vec3.h"

class AntQueen : public Ant
{

public:

    AntQueen( const AgentReport& report )
        :   Ant( report )
        ,   numberOfAntsLostInCombat( 0 )
        ,   hidding( false )
        ,   cornerToHide( positionX, positionY, 0.0 )
    {
        cornerToHide.x = cornerToHide.x < GameInfo::g_arenaInfo.width / 2 ? 1 : GameInfo::g_arenaInfo.width - 2;
        cornerToHide.y = cornerToHide.y < GameInfo::g_arenaInfo.height / 2 ? 1 : GameInfo::g_arenaInfo.height - 2;
    }

    virtual ~AntQueen(){}

    virtual OrderCode update( const AgentReport& report );

    int numberOfAntsLostInCombat;

    bool hidding;

    SBMath::vec3 cornerToHide;

private:



};