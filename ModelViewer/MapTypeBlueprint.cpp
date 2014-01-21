#include "MapTypeBlueprint.h"
#include "Map.h"

using namespace SBMath;
using namespace SBUtil;

namespace SBGame
{
    MapTypeBlueprint::MapTypeBlueprint( SBUtil::XMLNode& node )
    {
        validateBlueprint( *node.ToElement() );
        parseChild( *node.ToElement() );
    }

    void MapTypeBlueprint::validateBlueprint( TiXmlElement& element )
    {
        for( TiXmlElement* child = element.FirstChildElement(); child != 0; child = child->NextSiblingElement() )
        {
            validateBlueprint( *child );
        }

        XMLReader reader;

        if( strcmp( element.Value(), "MapType" ) == 0 )
        {
            reader.validateXMLAttributes( element, "name,default,size,numberOfPlayers", "" );
            reader.validateXMLChildElements( element, "", "Blob, Line, Resource" );                
        }

        else if( strcmp( element.Value(), "Blob" ) == 0 )
        {
            reader.validateXMLAttributes( element, "type,minSize,amount", "maxSize" );
        }

        else if( strcmp( element.Value(), "Line" ) == 0 )
        {
            reader.validateXMLAttributes( element, "type,minLength,amount", "maxLength" );
        }

        else if( strcmp( element.Value(), "Resource" ) == 0 )
        {
            reader.validateXMLAttributes( element, "type,amount", "localize,scale" );
        }
    }

    void MapTypeBlueprint::parseChild( TiXmlElement& element )
    {
        XMLReader reader;

        if( strcmp( element.Value(), "MapType" ) == 0 )
        {
            m_name = reader.getXMLAttributeAsString( element, "name", "" );
            m_default = reader.getXMLAttributeAsString( element, "default", "" );
            m_size = reader.getXMLAttributeAsIntVector2( element, "size", vec2i() );
            m_numberOfPlayers = reader.getXMLAttributeAsInt( element, "numberOfPlayers", 2 );
        }

        else if( strcmp( element.Value(), "Blob" ) == 0 )
        {
           Blob blob;
           
           blob.type = reader.getXMLAttributeAsString( element, "type", "" );
           blob.minSize = reader.getXMLAttributeAsInt( element, "minSize", 0 );
           blob.maxSize = reader.getXMLAttributeAsInt( element, "maxSize", blob.minSize );
           blob.amount = reader.getXMLAttributeAsInt( element, "amount", 0 );

           m_blobs.push_back( blob );
        }

        else if( strcmp( element.Value(), "Line" ) == 0 )
        {
            Line line;

            line.type = reader.getXMLAttributeAsString( element, "type", "" );
            line.minLength = reader.getXMLAttributeAsInt( element, "minLength", 0 );
            line.maxLength = reader.getXMLAttributeAsInt( element, "maxLength", line.minLength );
            line.amount = reader.getXMLAttributeAsInt( element, "amount", 0 );

            m_lines.push_back( line );
        }

        else if( strcmp( element.Value(), "Resource" ) == 0 )
        {
            Resource resource;

            resource.type = reader.getXMLAttributeAsString( element, "type", "" );
            resource.amount = reader.getXMLAttributeAsInt( element, "amount", 0 );
            resource.scale = reader.getXMLAttributeAsBool( element, "scale", false );            
            resource.localize = reader.getXMLAttributeAsBool( element, "localize", false );

            m_resources.push_back( resource );
        }

        for( TiXmlElement* child = element.FirstChildElement(); child != 0; child = child->NextSiblingElement() )
        {
            parseChild( *child );
        }
    }

    Map* MapTypeBlueprint::createMap( std::vector< TerrainBlueprint >& terrainBlueprints )
    {
        return new Map( *this, terrainBlueprints );
    }
}