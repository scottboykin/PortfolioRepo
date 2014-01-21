#pragma once

#include <string>
#include <vector>
#include "TerrainBlueprint.h"
#include "MapTypeBlueprint.h"
#include "vec2i.h"
#include "NamedProperty.h"

namespace SBGame
{
    class Terrain;

    class Map
    {

    public:

        Map( MapTypeBlueprint& blueprint, std::vector< TerrainBlueprint >& terrainBlueprints );
        ~Map();

        Terrain& getMapTile( int i );
        Terrain& getMapTile( int x, int y );
        int getNumberOfTiles(){ return m_size.x * m_size.y; }

        TerrainBlueprint* getTerrainBlueprint( const std::string& name, std::vector< TerrainBlueprint >& blueprints );
       
        void generateBlob( int x, int y, int size, TerrainBlueprint* type );
        void generateLine( int x, int y, int size, TerrainBlueprint* type );
        void generateResources( int x, int y, int amount, TerrainBlueprint* type );

        void loadMap( SBUtil::NamedProperty& properties );

        void render();

    private:

        const std::string* m_name;
        const std::string* m_default;

        SBMath::vec2i m_size;
        std::vector< SBMath::vec2i > m_playerStartPositions;

        const int m_numberOfPlayers;

        Terrain* m_mapTiles;

    };
    
}