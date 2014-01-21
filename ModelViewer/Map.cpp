#include <iostream>
#include "Map.h"
#include "Terrain.h"
#include "Utilities.h"
#include "CommandWindow.h"
#include "vec3.h"
#include "vec2.h"
#include "mat4x4.h"
#include "Renderer.h"

using namespace SBMath;
using namespace SBUtil;
using namespace SBGraphics;

namespace SBGame
{
    Map::Map( MapTypeBlueprint& blueprint, std::vector< TerrainBlueprint >& terrainBlueprints )
        :   m_name( &blueprint.getName() )
        ,   m_default( &blueprint.getDefault() )
        ,   m_size()
        ,   m_numberOfPlayers( blueprint.getNumberOfPlayer() )
    {
        TerrainBlueprint* defaultBlueprint = getTerrainBlueprint( *m_default, terrainBlueprints );

        if( !defaultBlueprint )
        {
            std::cout << "Default Terrain Blueprint not found" << std::endl;
            return;
        }

        m_size = vec2i( blueprint.getSize().x, blueprint.getSize().y );

        std::vector< vec2i > startLocations;
        for( int i = 1; i < 4; ++i )
        {
            for( int j = 1; j < 4; ++j )
            {
                if( !( j == i && j == 2 ) )
                    startLocations.push_back( vec2i( ( m_size.x / 4 ) * j, ( m_size.y / 4 ) * i ) );
            }
        }

        m_mapTiles = new Terrain[ m_size.x * m_size.y ];

        for( int row = 0; row < m_size.y; ++row )
        {
            for( int column = 0; column < m_size.x; ++column ) 
            {
                m_mapTiles[ row * m_size.x + column ] = Terrain( *defaultBlueprint );
            }
        }

        const std::vector< MapTypeBlueprint::Blob >& blobs = blueprint.getBlobs();
        for( unsigned int i = 0; i < blobs.size(); ++i )
        {
            for( int j = 0; j < blobs[i].amount; ++j )
            {
                int blobSize = (int)randInRange( (float)blobs[i].minSize, (float)blobs[i].maxSize );
                int x = (int)randInRange( 0, (float)m_size.x );
                int y = (int)randInRange( 0, (float)m_size.y );

                TerrainBlueprint* blobBlueprint = getTerrainBlueprint( blobs[i].type, terrainBlueprints );

                if( !blobBlueprint )
                {
                    continue;
                }

                generateBlob( x, y, blobSize, blobBlueprint );
            }
        }

        const std::vector< MapTypeBlueprint::Line >& lines = blueprint.getLines();
        for( unsigned int i = 0; i < lines.size(); ++i )
        {
            for( int j = 0; j < lines[i].amount; ++j )
            {
                int lineSize = (int)randInRange( (float)lines[i].minLength, (float)lines[i].maxLength );
                int x = (int)randInRange( 0, (float)m_size.x );
                int y = (int)randInRange( 0, (float)m_size.y );

                TerrainBlueprint* lineBlueprint = getTerrainBlueprint( lines[i].type, terrainBlueprints );

                if( !lineBlueprint )
                {
                    continue;
                }

                generateLine( x, y, lineSize, lineBlueprint );
            }
        }

        for( int i = 0; i < m_numberOfPlayers; ++i )
        {
            int position = randInRange( 0, startLocations.size() - 1 );
            getMapTile( startLocations[position].y * m_size.x + startLocations[position].x ) = Terrain( *getTerrainBlueprint( "playerStart", terrainBlueprints ) );
            m_playerStartPositions.push_back( startLocations[position] );
            startLocations.erase( startLocations.begin() + position );
        }

        const std::vector< MapTypeBlueprint::Resource > resources = blueprint.getResources();
        for( int i = 0; i < resources.size(); ++i )
        {
            if( resources[i].scale )
            {
                for( int j = 0; j < m_numberOfPlayers; ++j )
                {
                    if( resources[i].localize )
                    {
                        generateResources( m_playerStartPositions[j].x, m_playerStartPositions[j].y, resources[i].amount, getTerrainBlueprint( resources[i].type, terrainBlueprints ) );
                    }
                    else
                    {
                        for( int k = 0; k < resources[i].amount; ++k )
                        {
                            int x = randInRange( 0, m_size.x - 1 );
                            int y = randInRange( 0, m_size.y - 1 );

                            while( getMapTile( x, y ).getName()->compare( "playerStart" ) == 0 )
                            {
                                x = randInRange( 0, m_size.x - 1 );
                                y = randInRange( 0, m_size.y - 1 );
                            }

                            getMapTile( x, y ) = Terrain( *getTerrainBlueprint( resources[i].type, terrainBlueprints ) );
                        }
                    }
                }
            }
            else
            {
                if( resources[i].localize )
                {
                    int x = randInRange( 0, m_size.x - 1 );
                    int y = randInRange( 0, m_size.y - 1 );

                    generateResources( x, y, resources[i].amount, getTerrainBlueprint( resources[i].type, terrainBlueprints ) );
                }
                else
                {
                    for( int k = 0; k < resources[i].amount; ++k )
                    {
                        int x = randInRange( 0, m_size.x - 1 );
                        int y = randInRange( 0, m_size.y - 1 );

                        while( getMapTile( x, y ).getName()->compare( "playerStart" ) == 0 )
                        {
                            x = randInRange( 0, m_size.x - 1 );
                            y = randInRange( 0, m_size.y - 1 );
                        }

                        getMapTile( x, y ) = Terrain( *getTerrainBlueprint( resources[i].type, terrainBlueprints ) );
                    }
                }
            }
        }
    }

    Map::~Map()
    {
        delete[] m_mapTiles;
    }

    Terrain& Map::getMapTile( int i )
    {
        return ( i < 0 ) ? m_mapTiles[0] : ( i >= getNumberOfTiles() ) ? m_mapTiles[getNumberOfTiles() - 1] : m_mapTiles[i];
    }

    Terrain& Map::getMapTile( int x, int y )
    {
        return m_mapTiles[ y * m_size.x + x ];
    }

    void Map::render()
    {
        Renderer& renderer = Renderer::getRenderer();

        //renderer.projectionMatrixStack.pushMatrix();
        //renderer.viewMatrixStack.pushMatrix();

        renderer.projectionMatrixStack.getTop().orthoProjection( 0.0f, m_size.x, 0.0f, m_size.y, 0.0f, 1.0f );
        renderer.viewMatrixStack.loadIdentity();

        for( int row = 0; row < m_size.y; ++row )
        {
            for( int column = 0; column < m_size.x; ++column )
            {
                getMapTile( row * m_size.x + column ).render( column, row );
            }
        }

        //renderer.projectionMatrixStack.popMatrix();
        //renderer.viewMatrixStack.popMatrix();
    }

    TerrainBlueprint* Map::getTerrainBlueprint( const std::string& name, std::vector< TerrainBlueprint >& blueprints )
    {
        for( unsigned int i = 0; i < blueprints.size(); ++i )
        {
            if( blueprints[i].getName().compare( name ) == 0 )
            {
                return &blueprints[i];
            }
        }

        return 0;
    }

    void Map::generateBlob( int x, int y, int size, TerrainBlueprint* type )
    {
        vec2 center( x, y );

        for( int row = max( y - size, 0 ); row <= y + size; ++row )
        {
            for( int column = max( x - size, 0 ); column <= x + size; ++column )
            {
                vec2 placementPos( column, row );

                if( center.squaredDistance( placementPos ) < size * size )
                {
                    getMapTile( row * m_size.x + column ) = Terrain( *type );
                }
            }
        }
    }

    void Map::generateLine( int x, int y, int size, TerrainBlueprint* type )
    {
        int row = x;
        int column = y;

        for( int i = 0; i < size + 1; ++i )
        {
            getMapTile( row * m_size.x + column ) = Terrain( *type );

            int nextDirection = (int)randInRange( 0, 3);

            switch( nextDirection )
            {
                case 0: 
                    if( row - 1 >= 0 )
                    {
                        --row;
                    }
                    break;

                case 1:
                    if( column - 1 >= 0 )
                    {
                        --column;
                    }
                    break;

                case 2:
                    if( row + 1 < m_size.y )
                    {
                        ++row;
                    }
                    break;

                case 3:
                    if( column + 1 < m_size.x )
                    {
                        ++column;
                    }
                    break;
            }
        }
    }

    void Map::generateResources( int x, int y, int amount, TerrainBlueprint* type )
    {
        for( int i = 0; i < amount; ++i )
        {
            switch( i )
            {
            case 0: 
                getMapTile( ( y - 1 ) * m_size.x + x ) = Terrain( *type );
                break;

            case 1:
                getMapTile( ( y + 1 ) * m_size.x + x ) = Terrain( *type );
                break;

            case 2:
                getMapTile( y * m_size.x + x + 1 ) = Terrain( *type );
                break;

            case 3:
                getMapTile( y * m_size.x + x - 1 ) = Terrain( *type );
                break;
            }
        }
    }

    void Map::loadMap( NamedProperty& properties )
    {
        CommandWindow* window;
        
        properties.Get( std::string("console"), window );

        window->cprintf( "load map event called", SBMath::vec3( 1.0 ) );
    }
}