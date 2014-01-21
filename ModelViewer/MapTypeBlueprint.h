#pragma once

#include <string>
#include <vector>
#include <map>
#include "vec2i.h"
#include "TerrainBlueprint.h"
#include "XMLReader.h"

namespace SBGame
{
    class Map;

    class MapTypeBlueprint
    {

    public:

        struct Blob
        {
            std::string type;
            int minSize;
            int maxSize;
            int amount;
        };

        struct Line
        {
            std::string type;
            int minLength;
            int maxLength;
            int amount;
        };

        struct Resource
        {
            std::string type;
            int amount;
            bool localize;
            bool scale;
        };

        MapTypeBlueprint( SBUtil::XMLNode& node );

        void validateBlueprint( TiXmlElement& element );
        void parseChild( TiXmlElement& element );

        Map* createMap( std::vector< TerrainBlueprint >& terrainBlueprints );

        const std::string& getName() const { return m_name; } 
        const std::string& getDefault() const { return m_default; } 

        const int getNumberOfPlayer() const { return m_numberOfPlayers; }

        const SBMath::vec2i& getSize() const { return m_size; } 

        const std::vector< Blob >& getBlobs() const { return m_blobs; }
        const std::vector< Line >& getLines() const { return m_lines; }
        const std::vector< Resource >& getResources() const { return m_resources; }

    private:
        
        std::string m_name;
        std::string m_default;

        SBMath::vec2i m_size;

        int m_numberOfPlayers;

        std::vector< Blob > m_blobs;
        std::vector< Line > m_lines;
        std::vector< Resource > m_resources;

    };


}