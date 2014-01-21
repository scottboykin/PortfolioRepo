#pragma once

#include <list>
#include <vector>
#include <map>
#include "../Arena/include/CommonInterface.hpp"
#include "MapTile.h"
#include "vec3.h"

class Ant;
class AntQueen;

struct EnemyQueen
{
    EnemyQueen()
    {}

    SBMath::vec3 lastKnownLocation;
    int enemyPlayerID;
};

class GameInfo
{

public:

    enum Strategies
    {
        STRATEGY_SOLO,
        STRATEGY_1V1,
        STRATEGY_FFA
    };

    GameInfo()
    {
        for( int i = 0; i < 4; ++i )
        {
            numOfAntsPerType[i] = 0;
        }
    }

    static MapTile& GameInfo::getMapTile( const SBMath::vec3& position );
    static MapTile& GameInfo::getMapTile( int x, int y );
    static MapTile& GameInfo::getMapTile( const Ant& ant );

    static OrderCode getDirectionToMove( const SBMath::vec3& start, const SBMath::vec3& end );

    static int getManhattanDistance( const SBMath::vec3& a, const SBMath::vec3& b );

    static std::map< int, Ant* > myAnts;

    static AntQueen* g_myQueen;
    static ArenaInfo g_arenaInfo;

    static std::map< int, EnemyQueen > enemyQueens;

    static bool bSoldierSacrificed;
    static Strategies currentStrategy;
    static int numOfAntsPerType[4];
    static int g_playerID;
    static int numberOfNutrients;
    static int collectionRate;
    static int turnNumber;
    static int turnSinceLastSeenResource;
    static int amountOfFoodHeldByWorkers;

    static int totalAStarRequests;
    static double totalTimeSpentRequestingAStarPaths;

    static double secondsSinceLastFetchOrders;


    static const double timeLimit;

    static std::vector< MapTile > g_mapTiles;
    static std::list< MapTile* > g_resources;

};