#include "AntSoldier.h"
#include "AntQueen.h"
#include "RealTimeQuery.h"
#include "vec3.h"
#include "GameInfo.h"

using namespace SBUtil;
using namespace SBMath;

OrderCode AntSoldier::update( const AgentReport& report )
{
    OrderCode orderCode = Ant::update( report );

    if( path.size() > 0 )
    {
        orderCode = GameInfo::getDirectionToMove( vec3( positionX, positionY, 0.0f ), path.back()->getPosition() );
        path.pop_back();
    }

    else if( GameInfo::turnNumber >= 1000 )
    {
        vec3 queenPosition( GameInfo::g_myQueen->positionX, GameInfo::g_myQueen->positionY, 0.0 );

        if( vec3( positionX, positionY, 0.0 ) == queenPosition )
            orderCode = ORDER_HOLD;
        else
        {
            if( GetAbsoluteTimeSeconds() - GameInfo::secondsSinceLastFetchOrders < GameInfo::timeLimit )
            {
                requestAStarPath( &GameInfo::getMapTile( queenPosition ) );

                if( path.size() > 0 )
                {
                    orderCode = GameInfo::getDirectionToMove( vec3( positionX, positionY, 0.0f ), path.back()->getPosition() );
                    path.pop_back();
                }
            }
            else
            {
                orderCode = GameInfo::getDirectionToMove( vec3( positionX, positionY, 0.0f ), queenPosition );
                m_bWantsPath = true;
                m_PathTo = queenPosition;
            }
        }
    }

    if( GameInfo::turnNumber >= 1000 && !GameInfo::bSoldierSacrificed && GameInfo::numOfAntsPerType[ENTITY_TYPE_SOLDIER] > 5 )
    {
        GameInfo::bSoldierSacrificed = true;
        orderCode = ORDER_SUICIDE;
    }

    if( GameInfo::turnNumber < 1000 && !GameInfo::bSoldierSacrificed && GameInfo::numberOfNutrients < 750 )
    {
        GameInfo::bSoldierSacrificed = true;
        orderCode = ORDER_SUICIDE;
    }

    return orderCode;
}