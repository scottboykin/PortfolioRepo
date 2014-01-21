#pragma once

#include "AStarNode.h"
#include "../Arena/include/CommonInterface.hpp"

using namespace SBAI;

class MapTile : public AStarNode
{

public:

    MapTile()
        :    AStarNode( "air" )
        ,    m_type( ARENA_SQUARE_TYPE_AIR )
    {}

    MapTile( const char* arenaTypeString, ArenaSquareType arenaType )
        :    AStarNode( arenaTypeString )
        ,    m_type( arenaType )
    {}

    void setType( ArenaSquareType type )
    {
        m_type = type;
        switch( type )
        {
        case ARENA_SQUARE_TYPE_AIR:
            setNodeTypeID( "air" );
            break;

        case ARENA_SQUARE_TYPE_DIRT:
            setNodeTypeID( "dirt" );
            break;

        case  ARENA_SQUARE_TYPE_FOOD:
            setNodeTypeID( "food" );
            break;

        case ARENA_SQUARE_TYPE_STONE:
            setNodeTypeID( "stone" );
            break;

        case  ARENA_SQUARE_TYPE_UNKNOWN:
            setNodeTypeID( "unknown" );
            break;
        }
    }

    ArenaSquareType getType() const { return m_type; }


private:

    ArenaSquareType m_type;
};