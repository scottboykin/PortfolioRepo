#include "AntQueen.h"
#include "RealTimeQuery.h"

using namespace SBMath;
using namespace SBUtil;

OrderCode AntQueen::update( const AgentReport& report )
{
    OrderCode orderCode = Ant::update( report );

    switch( report.m_reportCode )
    {
    case REPORT_CREATE_SUCCESSFUL:
        
        break;

    case REPORT_MOVE_SUCCESSFUL:
        orderCode = ORDER_HOLD;

    default:
        break;
    }

    bool bAcceptableNutrients = GameInfo::numberOfNutrients > 2000;
    bool bCreateSoldier = numberOfAntsLostInCombat > 0 && bAcceptableNutrients && GameInfo::numOfAntsPerType[ENTITY_TYPE_SOLDIER] < GameInfo::numOfAntsPerType[ENTITY_TYPE_WORKER] / 2;
    bool bEndGameSoliderCapacity = GameInfo::turnNumber >= 1000 && GameInfo::numOfAntsPerType[ENTITY_TYPE_SOLDIER] < 5;

    
    if( GameInfo::currentStrategy != GameInfo::STRATEGY_SOLO && ( bCreateSoldier || bEndGameSoliderCapacity ) )
    {
        orderCode = ORDER_CREATE_SOLDIER;
        if( numberOfAntsLostInCombat > 0 )
            --numberOfAntsLostInCombat;
    }
    else
    {
        if( GameInfo::numberOfNutrients > 1500 && GameInfo::collectionRate > 2 * GameInfo::myAnts.size() && GameInfo::turnNumber < 1000 && GameInfo::turnNumber - GameInfo::turnSinceLastSeenResource <= 30 )
        {
            orderCode = ORDER_CREATE_WORKER;
        }

        else
        {
            if( GameInfo::amountOfFoodHeldByWorkers > 0 )
                orderCode = ORDER_HOLD;

            if( orderCode != ORDER_HOLD )
            {
                if( path.size() > 0 )
                {
                    orderCode = GameInfo::getDirectionToMove( vec3( positionX, positionY, 0.0f ), path.back()->getPosition() );
                    path.pop_back();
                }

                else
                {
                    unsigned int numberOfSeenResources = GameInfo::g_resources.size();
                    if( numberOfSeenResources > 0 )
                    {
                        vec3 antPos( positionX, positionY, 0 );
                        MapTile* closestResource = GameInfo::g_resources.front();
                        int distanceToClosest = GameInfo::getManhattanDistance( antPos, closestResource->getPosition() );

                        for( auto iter = ++GameInfo::g_resources.begin(); iter != GameInfo::g_resources.end(); ++iter )
                        {
                            int distanceCheck = GameInfo::getManhattanDistance( antPos, (*iter)->getPosition() );
                            if( distanceCheck < distanceToClosest )
                            {
                                distanceToClosest = distanceCheck;
                                closestResource = *iter;
                            }
                        }

                        GameInfo::g_resources.remove( closestResource );

                        if( GetAbsoluteTimeSeconds() - GameInfo::secondsSinceLastFetchOrders < GameInfo::timeLimit )
                        {
                            requestAStarPath( &GameInfo::getMapTile( closestResource->getPosition() ) );

                            if( path.size() > 0 )
                            {
                                orderCode = GameInfo::getDirectionToMove( vec3( positionX, positionY, 0.0f ), path.back()->getPosition() );
                                path.pop_back();
                            }
                        }
                        else
                        {
                            orderCode = GameInfo::getDirectionToMove( vec3( positionX, positionY, 0.0f ), closestResource->getPosition() );
                            m_bWantsPath = true;
                            m_PathTo = closestResource->getPosition();
                        }
                    }
                }
            }
        }
    }

    /*if( GameInfo::numOfAntsPerType[ENTITY_TYPE_WORKER] == 0 && GameInfo::turnNumber < 1000 )
    {
        orderCode = ORDER_CREATE_WORKER;
    }*/

    return orderCode;
}