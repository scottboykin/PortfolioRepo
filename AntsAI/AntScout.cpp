#include "AntScout.h"
#include "AntQueen.h"
#include "vec3.h"
#include "GameInfo.h"
#include "Utilities.h"
#include "RealTimeQuery.h"

using namespace SBUtil;
using namespace SBMath;

OrderCode AntScout::update( const AgentReport& report )
{
    OrderCode orderCode = Ant::update( report );

    if( path.size() > 0 )
    {
        orderCode = GameInfo::getDirectionToMove( vec3( positionX, positionY, 0.0f ), path.back()->getPosition() );
        path.pop_back();
    }

    else
    {
        int deltaX = randInRange( -10.0f, 10.0f );
        int deltaY = randInRange( -10.0f, 10.0f );

        vec3 scoutPosition( positionX + deltaX, positionY + deltaY, 0.0 );

        scoutPosition.x = clamp( scoutPosition.x, 0.0f, (float)GameInfo::g_arenaInfo.width - 1 );
        scoutPosition.y = clamp( scoutPosition.y, 0.0f, (float)GameInfo::g_arenaInfo.height - 1 );

        if( GetAbsoluteTimeSeconds() - GameInfo::secondsSinceLastFetchOrders < GameInfo::timeLimit )
        {
            requestAStarPath( &GameInfo::getMapTile( scoutPosition ) );

            if( path.size() > 0 )
            {
                orderCode = GameInfo::getDirectionToMove( vec3( positionX, positionY, 0.0f ), path.back()->getPosition() );
                path.pop_back();
            }
        }
        else
        {
            orderCode = GameInfo::getDirectionToMove( vec3( positionX, positionY, 0.0f ), scoutPosition );
            m_bWantsPath = true;
            m_PathTo = scoutPosition;
        }
    }

    return orderCode;
}