#include "AntWorker.h"
#include "AntQueen.h"
#include "MapTile.h"

using namespace SBUtil;
using namespace SBMath;

OrderCode AntWorker::update( const AgentReport& report )
{
    OrderCode orderCode = Ant::update( report );

    if( m_bWantsPath && GetAbsoluteTimeSeconds() - GameInfo::secondsSinceLastFetchOrders < GameInfo::timeLimit )
    {
        requestAStarPath( &GameInfo::getMapTile( (int)m_PathTo.x, (int)m_PathTo.y ) );
        m_bWantsPath = false;
    }

    switch( report.m_reportCode )
    {
        case REPORT_ERROR_BLOCKED_BY_ROCK:
            path.clear();
        case REPORT_ERROR_SQUARE_NOT_EMPTY:
        case REPORT_TAKE_SUCCESSFUL:
            if( GetAbsoluteTimeSeconds() - GameInfo::secondsSinceLastFetchOrders < GameInfo::timeLimit )
            {
                requestAStarPath( &GameInfo::getMapTile( *GameInfo::g_myQueen ) );

                if( path.size() > 0 )
                {
                    orderCode = GameInfo::getDirectionToMove( vec3( positionX, positionY, 0.0f ), path.back()->getPosition() );
                    path.pop_back();
                }
            }
            else
            {
                vec3 queenPos( GameInfo::g_myQueen->positionX, GameInfo::g_myQueen->positionY, 0.0f );
                orderCode = GameInfo::getDirectionToMove( vec3( positionX, positionY, 0.0f ), queenPos );
                m_bWantsPath = true;
                m_PathTo = queenPos;
            }
            break;

        default:

            if( path.size() > 0 )
            {
				orderCode = GameInfo::getDirectionToMove( vec3( positionX, positionY, 0.0f ), path.back()->getPosition() );
				path.pop_back();
            }
            else
            {
                if( specialStatus == ENTITY_SPECIAL_STATUS_CARRYING_FOOD )
                {
                    orderCode = ORDER_DROP_FOOD;
                    --GameInfo::amountOfFoodHeldByWorkers;
                }
                else
                {
                    ArenaSquareType squareType = GameInfo::getMapTile( *this ).getType();
                    if( squareType == ARENA_SQUARE_TYPE_FOOD )
                    {
                        orderCode = ORDER_TAKE_FOOD;
                        ++GameInfo::amountOfFoodHeldByWorkers;
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
                            claimedResource = closestResource;

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

            if( GameInfo::turnNumber - GameInfo::turnSinceLastSeenResource > 30 && specialStatus != ENTITY_SPECIAL_STATUS_CARRYING_FOOD )
            {
                orderCode = ORDER_SUICIDE;
            }
            
            break;
    }

    return orderCode;
}

void AntWorker::ProcessDeath()
{
    if( claimedResource )
    {
        GameInfo::g_resources.push_back( claimedResource );
    }

    if( specialStatus == ENTITY_SPECIAL_STATUS_CARRYING_FOOD )
    {
        --GameInfo::amountOfFoodHeldByWorkers;
    }
}