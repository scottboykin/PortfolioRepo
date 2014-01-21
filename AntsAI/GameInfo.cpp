#include "GameInfo.h"
#include "Ant.h"
#include "AntQueen.h"

using namespace SBMath;

AntQueen* GameInfo::g_myQueen;
std::map< int, Ant* > GameInfo::myAnts;

std::map< int, EnemyQueen > GameInfo::enemyQueens;

ArenaInfo GameInfo::g_arenaInfo;

bool GameInfo::bSoldierSacrificed = false;
GameInfo::Strategies GameInfo::currentStrategy = STRATEGY_FFA;
int GameInfo::numOfAntsPerType[4];
int GameInfo::g_playerID = 0;
int GameInfo::numberOfNutrients = 0;
int GameInfo::collectionRate = 0;
int GameInfo::turnNumber = 0;
int GameInfo::turnSinceLastSeenResource = 0;
int GameInfo::totalAStarRequests = 0;
int GameInfo::amountOfFoodHeldByWorkers = 0;
double GameInfo::totalTimeSpentRequestingAStarPaths = 0.0;
double GameInfo::secondsSinceLastFetchOrders = 0.0;
const double GameInfo::timeLimit = 0.02;

std::vector< MapTile > GameInfo::g_mapTiles;
std::list< MapTile* > GameInfo::g_resources;

MapTile& GameInfo::getMapTile( const vec3& position )
{
    return g_mapTiles[ (int)position.y * g_arenaInfo.width + (int)position.x ];
}

MapTile& GameInfo::getMapTile( int x, int y )
{
    return g_mapTiles[ y * g_arenaInfo.width + x ];
}

MapTile& GameInfo::getMapTile( const Ant& ant )
{
    return g_mapTiles[ ant.positionY * g_arenaInfo.width + ant.positionX ];
}

OrderCode GameInfo::getDirectionToMove( const vec3& start, const vec3& end )
{
    vec3 direction = end - start;

    if( direction.x < 0 )
    {
        return ORDER_MOVE_WEST;
    }
    else if( direction.x > 0 )
    {
        return ORDER_MOVE_EAST;
    }
    else if( direction.y < 0 )
    {
        return ORDER_MOVE_NORTH;
    }
    else if( direction.y > 0 )
    {
        return ORDER_MOVE_SOUTH;
    }

    return ORDER_HOLD;

}

int GameInfo::getManhattanDistance( const SBMath::vec3& a, const SBMath::vec3& b )
{
    return (int)( abs( b.x - a.x ) + abs( b.y - a.y ) + abs( b.z - a.z ) );
}