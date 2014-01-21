#include "Ant.h"
#include "AStarNode.h"
#include "AStarPathData.h"
#include "RealTimeQuery.h"
#include "GameInfo.h"

using namespace SBAI;
using namespace SBUtil;

Ant::Ant( const AgentReport& report )
    :   entityID( report.m_entityID )
    ,   entityType( (EntityType)report.m_agentType )
    ,   positionX( report.m_newPositionX )
    ,   positionY( report.m_newPositionY )
    ,   reportCode( (ReportCode)report.m_reportCode )
    ,   specialStatus( (EntitySpecialStatus)report.m_specialStatus )
    ,   m_bWantsPath( false )
{}

OrderCode Ant::update( const AgentReport& report )
{
    OrderCode orderCode = ORDER_HOLD;

    positionX = report.m_newPositionX;
    positionY = report.m_newPositionY;
    specialStatus = (EntitySpecialStatus)report.m_specialStatus;
    reportCode = (ReportCode)report.m_reportCode;

    orderCode = (OrderCode)( ORDER_MOVE_EAST + (rand() % (((int) NUM_CARDINAL_DIRECTIONS)-1)) );

    return orderCode;
}

void Ant::requestAStarPath( AStarNode* destination )
{
    double pathingStartTime = GetAbsoluteTimeSeconds();
    GameInfo::getMapTile( *this ).generatePath( destination, path );
    GameInfo::totalTimeSpentRequestingAStarPaths += ( GetAbsoluteTimeSeconds() - pathingStartTime );
    ++GameInfo::totalAStarRequests;
}