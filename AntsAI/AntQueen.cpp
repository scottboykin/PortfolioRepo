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

	int currentNumOfSoliders = GameInfo::numOfAntsPerType[ENTITY_TYPE_SOLDIER];
	int currentNumOfWorkers = GameInfo::numOfAntsPerType[ENTITY_TYPE_WORKER];
	
    bool endGame =  GameInfo::turnNumber >= 1000;
    bool acceptableNutrients = GameInfo::numberOfNutrients > 2000;

    bool createSoldier = numberOfAntsLostInCombat > 0 && acceptableNutrients && currentNumOfSoliders < currentNumOfWorkers / 2;
    bool endGameSoliderCapacity = endGame && currentNumOfSoliders < 5;
    bool noMoreResourcesFound = GameInfo::turnNumber - GameInfo::turnSinceLastSeenResource <= 30;

    
    if( GameInfo::currentStrategy != GameInfo::STRATEGY_SOLO && ( createSoldier || endGameSoliderCapacity ) )
    {
        orderCode = ORDER_CREATE_SOLDIER;
        if( numberOfAntsLostInCombat > 0 )
            --numberOfAntsLostInCombat;
    }
    else
    {
        if( GameInfo::numberOfNutrients > 1500 && GameInfo::collectionRate > 2 * GameInfo::myAnts.size() && !endGame && noMoreResourcesFound )
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

    return orderCode;
}