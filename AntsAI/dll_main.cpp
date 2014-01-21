#include <stdio.h>
#include <vector>
#include <string>
#include <stdarg.h>
#include <windows.h>
#include <list>
#include <crtdbg.h>
#include <vadefs.h>
#include "../Arena/include/CommonInterface.hpp"
#include "AStarNode.h"
#include "AStarPathData.h"
#include "vec3.h"
#include "RealTimeQuery.h"
#include "Utilities.h"
#include "Ant.h"
#include "AntScout.h"
#include "AntSoldier.h"
#include "AntQueen.h"
#include "AntWorker.h"
#include "MapTile.h"
#include "GameInfo.h"

// These opengl includes are only necessary if you want to draw your own debugging overlay graphics by implementing DrawDebugOverlay().
#include <gl/gl.h>
#include <gl/glu.h>
#pragma comment( lib, "opengl32" ) // Link in the OpenGL32.lib static library
#pragma comment( lib, "Glu32" ) // Link in the Glu32.lib static library

using namespace SBMath;
using namespace SBAI;
using namespace SBUtil;

bool g_isQuitting = false;
bool g_areNewUpdatesReadyToProcess = false;
bool g_readyToDispatchOrders = false;

GameInfo gameInfo;

std::map< int, Ant* >& g_myAnts = gameInfo.myAnts;

AgentReports g_resultsForPreviousOrders;
ObservedEntities g_observedEnemyEntities;
Orders g_tentativeOrders;

CRITICAL_SECTION cs_ordersAndResults;

//-----------------------------------------------------------------------------------------------
int GetInterfaceVersion()
{
//	DebuggerPrintf( "[?] GetInterfaceVersion called.\n" );
	return COMMON_INTERFACE_VERSION_NUMBER;
};


//-----------------------------------------------------------------------------------------------
const char* GetPlayerName()
{
//	DebuggerPrintf( "[?] GetPlayerName called.\n" );
	return "Hank Pym";
};


//-----------------------------------------------------------------------------------------------
const char* GetAuthorName()
{
//	DebuggerPrintf( "[?] GetAuthorName called.\n" );
	return "Scott Boykin";
};


//-----------------------------------------------------------------------------------------------
void PrepareForBattle( int playerID, const ArenaInfo& arenaInfo )
{
	InitializeCriticalSection( &cs_ordersAndResults );

	gameInfo.g_playerID = playerID;
    gameInfo.g_arenaInfo = arenaInfo;
    gameInfo.secondsSinceLastFetchOrders = GetAbsoluteTimeSeconds();

    if( gameInfo.g_arenaInfo.numPlayers == 1 )
        gameInfo.currentStrategy = GameInfo::STRATEGY_SOLO;

    gameInfo.g_mapTiles = std::vector< MapTile >( arenaInfo.width * arenaInfo.height );
    vec3 mapSize( arenaInfo.width, arenaInfo.height, 0.0f );

    MapTile initializingDefaultGCosts;
    initializingDefaultGCosts.setNodeTypeID( "dirt" );
    initializingDefaultGCosts.setDefaultGCost( 0.5f );
    initializingDefaultGCosts.setNodeTypeID( "stone" );
    initializingDefaultGCosts.setDefaultGCost( 0.0f );
    initializingDefaultGCosts.setNodeTypeID( "food" );
    initializingDefaultGCosts.setNodeTypeID( "unknown" );

    for( int y = 0; y < arenaInfo.height; ++y )
    {
        for( int x = 0; x < arenaInfo.width; ++x )
        {
            MapTile& currentTile = gameInfo.g_mapTiles[ y * arenaInfo.width + x ];
            currentTile.setType( ARENA_SQUARE_TYPE_UNKNOWN );
            currentTile.setPosition( vec3( x, y, 0.0f ) );

            for( int sign = -1; sign <= 1; sign += 2 )
            {
                for( int axis = 0; axis < 2; ++axis)
                {
                    vec3 neighorPos( x, y, 0.0f );
                    neighorPos[axis] += sign;

                    if( neighorPos[axis] >= 0 && neighorPos[axis] < mapSize[axis] )
                    {
                        currentTile.addNeighbor( &gameInfo.g_mapTiles[neighorPos.y * mapSize.x + neighorPos.x] );
                    }
                }
            }
        }
    }

	//DebuggerPrintf( "[%d] PrepareForBattle called; arena is %dx%d, with %d players\n", g_playerID, arenaInfo.width, arenaInfo.height, arenaInfo.numPlayers );
}


//-----------------------------------------------------------------------------------------------
void BeginWork()
{
//	DebuggerPrintf( "[%d] WORKER THREAD: BeginWork() called.\n", g_playerID );

	while( !g_isQuitting )
	{
		EnterCriticalSection( &cs_ordersAndResults );
		bool areTentativeOrdersEmpty = g_tentativeOrders.m_numberOfOrders == 0;
		bool areResultsEmpty = g_resultsForPreviousOrders.m_numberAgentReports == 0;
		LeaveCriticalSection( &cs_ordersAndResults );

		bool readyToStartGeneratingNewOrders = areTentativeOrdersEmpty && !areResultsEmpty;
		if( !readyToStartGeneratingNewOrders )
		{
			EnterCriticalSection( &cs_ordersAndResults );
			g_readyToDispatchOrders = true;
			LeaveCriticalSection( &cs_ordersAndResults );
			Sleep( 0 );
		}
		else
		{
            EnterCriticalSection( &cs_ordersAndResults );
			Orders tempOrdersWhileWeDoLotsOfWork;
			int numOrdersToGive = (int) g_resultsForPreviousOrders.m_numberAgentReports;

			for( int resultIndex = 0; resultIndex < numOrdersToGive; ++ resultIndex )
			{
                const AgentReport& result = g_resultsForPreviousOrders.m_agentReports[ resultIndex ];
                int antID = result.m_entityID;
                OrderCode orderCode = ORDER_HOLD;

                if( result.m_specialStatus == ENTITY_SPECIAL_STATUS_DEAD )
                {
                    if( result.m_reportCode == REPORT_WAS_KILLED_IN_COMBAT )
                        ++GameInfo::g_myQueen->numberOfAntsLostInCombat;

                    if( g_myAnts.find( antID ) != g_myAnts.end() )
                    {
                        g_myAnts[antID]->ProcessDeath();
                        --gameInfo.numOfAntsPerType[(int)result.m_agentType];
                        delete g_myAnts[antID];
                        g_myAnts.erase( antID );
                        continue;
                    }
                }

                else
                {
                    if( result.m_reportCode == REPORT_WAS_CREATED )
                    {
                        switch( result.m_agentType )
                        {
                        case ENTITY_TYPE_SCOUT: 
                            g_myAnts[antID] = new AntScout( result );
                            break;

                        case ENTITY_TYPE_SOLDIER:
                            g_myAnts[antID] = new AntSoldier( result );
                            break;

                        case ENTITY_TYPE_QUEEN: 
                            gameInfo.g_myQueen = new AntQueen( result );
                            g_myAnts[antID] = gameInfo.g_myQueen;
                            break;

                        case ENTITY_TYPE_WORKER: 
                            g_myAnts[antID] = new AntWorker( result );
                            break;
                        }
                        ++gameInfo.numOfAntsPerType[(int)result.m_agentType];
                    }

                    if( gameInfo.myAnts.find( antID ) != gameInfo.myAnts.end() )
                        orderCode = g_myAnts[antID]->update( result );

                    tempOrdersWhileWeDoLotsOfWork.AddOrder( antID, orderCode, true );
                }
			}
            
            //float timeSpent = GetAbsoluteTimeSeconds() - gameInfo.secondsSinceLastFetchOrders;
            //DebuggerPrintf( "Milliseconds Spent: %f\n", timeSpent * 1000 );

            /*if( GameInfo::totalAStarRequests )
            {
                DebuggerPrintf( "Average Milliseconds Spent: %f\n", ( GameInfo::totalTimeSpentRequestingAStarPaths / GameInfo::totalAStarRequests ) * 1000 );
            }*/
            GameInfo::bSoldierSacrificed = false;
            g_readyToDispatchOrders = true;
			g_tentativeOrders = tempOrdersWhileWeDoLotsOfWork;
            g_resultsForPreviousOrders.m_numberAgentReports = 0;
			LeaveCriticalSection( &cs_ordersAndResults );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void EndWork()
{
	g_isQuitting = true;
}


//-----------------------------------------------------------------------------------------------
// Dispatch new orders to the Arena
//-----------------------------------------------------------------------------------------------
void FetchNewOrders( int turnNumber, Orders& newOrders_out )
{
    EnterCriticalSection( &cs_ordersAndResults );
    bool readyToDispatchOrders = g_readyToDispatchOrders;
    LeaveCriticalSection( &cs_ordersAndResults );

    while( !readyToDispatchOrders )
    {
        Sleep( 0 );
        EnterCriticalSection( &cs_ordersAndResults );
        readyToDispatchOrders = g_readyToDispatchOrders;
        LeaveCriticalSection( &cs_ordersAndResults );
    }

    EnterCriticalSection( &cs_ordersAndResults );
    newOrders_out = g_tentativeOrders;
    g_tentativeOrders.m_numberOfOrders = 0;
    g_readyToDispatchOrders = false;
    LeaveCriticalSection( &cs_ordersAndResults );
}

//-----------------------------------------------------------------------------------------------
// Update the colony's current state and get the results of the previous turn's orders
// Store the colony's current nutrient supply and its map of the arena
//-----------------------------------------------------------------------------------------------
void SendUpdate( int turnNumber, int currentNutrients, const AgentReports& agentReports, const ObservedEntities& observedEntities, const ArenaSquares& observedSquares )
{
//	DebuggerPrintf( "   AI {%d}: Received updates: %d nutrients, %d reports, and %d observed entities.\n", turnNumber, currentNutrients, agentReports.m_numberAgentReports, observedEntities.m_numberOfObservedEntities );
//	observedSquares.DebugDumpContents();

	EnterCriticalSection( &cs_ordersAndResults );
	g_resultsForPreviousOrders = agentReports;
	g_observedEnemyEntities = observedEntities;
    gameInfo.collectionRate = currentNutrients - gameInfo.numberOfNutrients;
	gameInfo.numberOfNutrients = currentNutrients;
    gameInfo.turnNumber = turnNumber;

    gameInfo.g_resources.clear();

    for( short y = 0; y < gameInfo.g_arenaInfo.height; ++y )
    {
        for( short x = 0; x < gameInfo.g_arenaInfo.width; ++x )
        {
            ArenaSquareType observedType = observedSquares.GetSquareTypeAtCoords( x, y );
            MapTile& currentTile = gameInfo.g_mapTiles[y * gameInfo.g_arenaInfo.width + x];

            if( observedType != ARENA_SQUARE_TYPE_UNKNOWN && observedType != currentTile.getType() )
            {
                currentTile.setType( observedType );
            }

            if( observedType == ARENA_SQUARE_TYPE_FOOD )
            {
                gameInfo.g_resources.push_back( &currentTile );
            }
        }
    }

    if( gameInfo.g_resources.size() > 0 )
        gameInfo.turnSinceLastSeenResource = turnNumber;

    gameInfo.secondsSinceLastFetchOrders = GetAbsoluteTimeSeconds();
    
	LeaveCriticalSection( &cs_ordersAndResults );
}


//-----------------------------------------------------------------------------------------------
void DrawDebugOverlay()
{
	glColor4ub( 0, 255, 0, 255 );
	glBegin( GL_LINES );
	{
		glVertex2f( 10.f, 10.f );
		glVertex2f( 30.f, 20.f );
	}
	glEnd();
}


