#pragma once

#include <vector>
#include "../Arena/include/CommonInterface.hpp"
#include "GameInfo.h"
#include "RealTimeQuery.h"
#include "vec3.h"

namespace SBAI
{
    class AStarNode;
}

class Ant
{

public:

    Ant() {}
    Ant( const AgentReport& report );
    virtual ~Ant(){}

    virtual OrderCode update( const AgentReport& report );
    virtual void ProcessDeath(){}

    int entityID;
    EntityType entityType;
    short positionX;
    short positionY;
    ReportCode reportCode;
    EntitySpecialStatus specialStatus;

    std::vector< SBAI::AStarNode* > path;

    void requestAStarPath( SBAI::AStarNode* destination  );

protected:

    bool m_bWantsPath;
    SBMath::vec3 m_PathTo;

};