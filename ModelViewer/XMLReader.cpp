#include <vector>
#include <iostream>
#include <sstream>
#include <map>
#include <Windows.h>
#include "XMLReader.h"
#include "StringParser.h"
#include "Utilities.h"

namespace SBUtil
{
    XMLReader::XMLReader()
    {}

    XMLReader::~XMLReader()
    {}

    //TODO
    /*TiXmlElement& XMLReader::getNextElement()
    {
        while( !currentElement->NoChildren() )
        {

        }
    }*/

    TiXmlDocument XMLReader::loadXMLDocument( std::string filePath )
    {
        TiXmlDocument xmlDoc( filePath.c_str() );
        xmlDoc.LoadFile();

        return xmlDoc;
    }

    bool XMLReader::getXMLAttributeAsBool( const TiXmlElement& element, const char* attributeName, bool defaultValueIfAttributeNotFound )
    {
        const TiXmlAttribute* pAttrib = element.FirstAttribute();
        
        while( pAttrib )
        {
            if( strcmp( pAttrib->Name(), attributeName ) == 0 )
            {
                if( strcmp( pAttrib->Value(), "true" ) == 0 )
                {
                    return true;
                }
                else if( strcmp( pAttrib->Value(), "false" ) == 0 )
                {
                    return false;
                }
                else
                {
                    return defaultValueIfAttributeNotFound;
                }
            }
            pAttrib = pAttrib->Next();
        }

        return defaultValueIfAttributeNotFound;
    }

    int XMLReader::getXMLAttributeAsInt( const TiXmlElement& element, const char* attributeName, int defaultValueIfAttributeNotFound )
    {
        const TiXmlAttribute* pAttrib = element.FirstAttribute();

        while( pAttrib )
        {
            if( strcmp( pAttrib->Name(), attributeName ) == 0 )
            {
                return atoi( pAttrib->Value() );
            }

            pAttrib = pAttrib->Next();
        }

        return defaultValueIfAttributeNotFound;
    }

    float XMLReader::getXMLAttributeAsFloat( const TiXmlElement& element, const char* attributeName, float defaultValueIfAttributeNotFound )
    {
        const TiXmlAttribute* pAttrib = element.FirstAttribute();

        while( pAttrib )
        {
            if( strcmp( pAttrib->Name(), attributeName ) == 0 )
            {
                return (float)atof( pAttrib->Value() );
            }

            pAttrib = pAttrib->Next();
        }

        return defaultValueIfAttributeNotFound;
    }

    SBGraphics::Color XMLReader::getXMLAttributeAsColor( const TiXmlElement& element, const char* attributeName, SBGraphics::Color defaultValueIfAttributeNotFound )
    {
        const TiXmlAttribute* pAttrib = element.FirstAttribute();

        while( pAttrib )
        {
            if( strcmp( pAttrib->Name(), attributeName ) == 0 )
            {
                StringParser sp( std::string( pAttrib->Value() ), std::string( "," ) );
                int r, g, b, a;
                
                r = atoi( sp.getNextToken().c_str() );
                g = atoi( sp.getNextToken().c_str() );
                b = atoi( sp.getNextToken().c_str() );
                a = atoi( sp.getNextToken().c_str() );

                return SBGraphics::Color( r, g, b, a );
            }

            pAttrib = pAttrib->Next();
        }

        return defaultValueIfAttributeNotFound;
    }

    SBMath::vec2 XMLReader::getXMLAttributeAsVector2( const TiXmlElement& element, const char* attributeName, SBMath::vec2 defaultValueIfAttributeNotFound )
    {
        const TiXmlAttribute* pAttrib = element.FirstAttribute();

        while( pAttrib )
        {
            if( strcmp( pAttrib->Name(), attributeName ) == 0 )
            {
                float x, y;
                StringParser sp( std::string( pAttrib->Value() ), std::string( "," ) );

                x = (float)atof( sp.getNextToken().c_str() );
                y = (float)atof( sp.getNextToken().c_str() );

                return SBMath::vec2( x, y );
            }

            pAttrib = pAttrib->Next();
        }

        return defaultValueIfAttributeNotFound;
    }

    SBMath::vec3 XMLReader::getXMLAttributeAsVector3( const TiXmlElement& element, const char* attributeName, SBMath::vec3 defaultValueIfAttributeNotFound )
    {
        const TiXmlAttribute* pAttrib = element.FirstAttribute();

        while( pAttrib )
        {
            if( strcmp( pAttrib->Name(), attributeName ) == 0 )
            {
                float x, y, z;
                StringParser sp( std::string( pAttrib->Value() ), std::string( "," ) );

                x = (float)atof( sp.getNextToken().c_str() );
                y = (float)atof( sp.getNextToken().c_str() );
                z = (float)atof( sp.getNextToken().c_str() );

                return SBMath::vec3( x, y, z );
            }

            pAttrib = pAttrib->Next();
        }

        return defaultValueIfAttributeNotFound;
    }

    SBMath::vec2i XMLReader::getXMLAttributeAsIntVector2( const TiXmlElement& element, const char* attributeName, SBMath::vec2i defaultValueIfAttributeNotFound )
    {
        const TiXmlAttribute* pAttrib = element.FirstAttribute();

        while( pAttrib )
        {
            if( strcmp( pAttrib->Name(), attributeName ) == 0 )
            {
                StringParser sp( std::string( pAttrib->Value() ), std::string( "," ) );
                int x, y;

                x = atoi( sp.getNextToken().c_str() );
                y = atoi( sp.getNextToken().c_str() );

                return SBMath::vec2i( x, y );
            }

            pAttrib = pAttrib->Next();
        }

        return defaultValueIfAttributeNotFound;
    }

    SBMath::vec3i XMLReader::getXMLAttributeAsIntVector3( const TiXmlElement& element, const char* attributeName, SBMath::vec3i defaultValueIfAttributeNotFound )
    {
        const TiXmlAttribute* pAttrib = element.FirstAttribute();

        while( pAttrib )
        {
            if( strcmp( pAttrib->Name(), attributeName ) == 0 )
            {
                StringParser sp( std::string( pAttrib->Value() ), std::string( "," ) );
                int x, y, z;

                x = atoi( sp.getNextToken().c_str() );
                y = atoi( sp.getNextToken().c_str() );
                z = atoi( sp.getNextToken().c_str() );

                return SBMath::vec3i( x, y, z );
            }

            pAttrib = pAttrib->Next();
        }

        return defaultValueIfAttributeNotFound;
    }

    std::string XMLReader::getXMLAttributeAsString( const TiXmlElement& element, const char* attributeName, std::string defaultValueIfAttributeNotFound )
    {
        const TiXmlAttribute* pAttrib = element.FirstAttribute();

        while( pAttrib )
        {
            if( strcmp( pAttrib->Name(), attributeName ) == 0 )
            {
                return pAttrib->Value();
            }

            pAttrib = pAttrib->Next();
        }

        return defaultValueIfAttributeNotFound;
    }

    std::string XMLReader::getXMLElementPCDataAsString( const TiXmlElement& element )
    {
        const char* pcData = element.GetText();
        if( pcData )
        {
            return std::string( pcData );
        }

        return std::string();
    }

    void XMLReader::validateXMLChildElements( const TiXmlElement& element, const char* commaSeparatedListOfRequiredChildren, const char* commaSeparatedListOfOptionalChildren )
    {
        std::map< std::string, bool > required;
        std::vector< std::string > optional;

        StringParser requiredParser( std::string( commaSeparatedListOfRequiredChildren ), std::string( "," ) );
        StringParser optionalParser( std::string( commaSeparatedListOfOptionalChildren ), std::string( "," ) );

        while( !requiredParser.stringFullyParsed() )
        {
            required.insert( std::pair< std::string, bool >( requiredParser.getNextToken(), false ) );
        }

        while( !optionalParser.stringFullyParsed() )
        {
            optional.push_back( optionalParser.getNextToken() );
        }

        for( const TiXmlElement* pChild = element.FirstChildElement(); pChild != 0; pChild = pChild->NextSiblingElement() )
        {  
            bool found = false;
            for( auto iter = required.begin(); iter != required.end(); ++iter )
            {
                if( strcmp( pChild->Value(), iter->first.c_str() ) == 0 )
                {
                    iter->second = true;
                    found = true;
                    break;
                }

                else if( strcmp( StringParser::toLower( pChild->Value() ).c_str(), StringParser::toLower( iter->first.c_str() ).c_str() ) == 0 )
                {
                    errorCaseMismatchElement( element, pChild->Value(), pChild->Row(), iter->first );
                }
            }

            if( !found )
            {
                for( auto iter = optional.begin(); iter != optional.end(); ++iter )
                {
                    if( strcmp( pChild->Value(), iter->c_str() ) == 0 )
                    {
                        found = true;
                        break;
                    }

                    else if( strcmp( StringParser::toLower( pChild->Value() ).c_str(), StringParser::toLower( iter->c_str() ).c_str() ) == 0 )
                    {
                        errorCaseMismatchElement( element, pChild->Value(), pChild->Row(), *iter );
                    }
                }

                if( !found )
                {
                    errorInvalidElement( element, pChild->Value(), pChild->Row(), required, optional );
                }
            }
        }

        for( auto iter = required.begin(); iter != required.end(); ++iter )
        {
            if( !iter->second )
            {
                errorMissingRequiedElement( element, iter->first, element.Row() );
            }
        }
    }

    void XMLReader::validateXMLAttributes( const TiXmlElement& element, const char* commaSeparatedListOfRequiredAttributes, const char* commaSeparatedListOfOptionalAttributes )
    {
        std::map< std::string, bool > required;
        std::vector< std::string > optional;
        
        StringParser requiredParser( std::string( commaSeparatedListOfRequiredAttributes ), std::string( "," ) );
        StringParser optionalParser( std::string( commaSeparatedListOfOptionalAttributes ), std::string( "," ) );

        while( !requiredParser.stringFullyParsed() )
        {
            required.insert( std::pair< std::string, bool >( requiredParser.getNextToken(), false ) );
        }

        while( !optionalParser.stringFullyParsed() )
        {
            optional.push_back( optionalParser.getNextToken() );
        }

        for( const TiXmlAttribute* pChild = element.FirstAttribute(); pChild != 0; pChild = pChild->Next() )
        {  
            bool found = false;
            for( auto iter = required.begin(); iter != required.end(); ++iter )
            {
                if( strcmp( pChild->Name(), iter->first.c_str() ) == 0 )
                {
                    iter->second = true;
                    found = true;
                    break;
                }

                else if( strcmp( StringParser::toLower( pChild->Name() ).c_str(), StringParser::toLower( iter->first.c_str() ).c_str() ) == 0 )
                {
                    errorCaseMismatchAttrib( element, pChild->Name(), pChild->Row(), iter->first );
                }
            }

            if( !found )
            {
                for( auto iter = optional.begin(); iter != optional.end(); ++iter )
                {
                    if( strcmp( pChild->Name(), iter->c_str() ) == 0 )
                    {
                        found = true;
                        break;
                    }

                    else if( strcmp( StringParser::toLower( pChild->Name() ).c_str(), StringParser::toLower( iter->c_str() ).c_str() ) == 0 )
                    {
                        errorCaseMismatchAttrib( element, pChild->Name(), pChild->Row(), *iter );
                    }
                }

                if( !found )
                {
                    errorInvalidAttrib( element, pChild->Name(), pChild->Row(), required, optional );
                }
            }
        }

        for( auto iter = required.begin(); iter != required.end(); ++iter )
        {
            if( !iter->second )
            {
                errorMissingRequiedAttrib( element, iter->first, element.Row() );
            }
        }
    }

    void XMLReader::errorMissingRequiedElement( const TiXmlElement& parent, const std::string& name, int row )
    {
        char filePath[250];
        char stringBuffer[250];
        std::string error = "Missing required element \"" + name + "\"";

        sprintf( filePath, "%s\\%s (%i) : %s\n", getCurrentDirectory().c_str(), parent.GetDocument()->Value(), row, error.c_str() );

        OutputDebugStringA( filePath );
        sprintf( stringBuffer, "Missing a required element \"%s\" in file \"%s\".", name.c_str(), filePath );
        MessageBoxA( NULL, stringBuffer, "Missing Required Element", MB_OK | MB_ICONERROR | MB_TOPMOST );

        __debugbreak();
        exit( 1 );
    }

    void XMLReader::errorMissingRequiedAttrib( const TiXmlElement& parent, const std::string& name, int row )
    {
        char filePath[250];
        char stringBuffer[250];
        std::string error = "Missing required attribute \"" + name + "\"";

        sprintf( filePath, "%s\\%s (%i) : %s\n", getCurrentDirectory().c_str(), parent.GetDocument()->Value(), row, error.c_str() );

        OutputDebugStringA( filePath );
        sprintf( stringBuffer, "Missing a required attribute \"%s\" in file \"%s\".", name.c_str(), filePath );
        MessageBoxA( NULL, stringBuffer, "Missing Required Attribute", MB_OK | MB_ICONERROR | MB_TOPMOST );

        __debugbreak();
        exit( 1 );
    }

    void XMLReader::errorInvalidElement( const TiXmlElement& parent, const std::string& name, int row, const std::map< std::string, bool >& required, const std::vector< std::string >& optional )
    {
        char filePath[250];
        char stringBuffer[250];
        std::string error = "Invalid element \"" + name + "\"";

        sprintf( filePath, "%s\\%s (%i) : %s\n", getCurrentDirectory().c_str(), parent.GetDocument()->Value(), row, error.c_str() );

        OutputDebugStringA( filePath );
        sprintf( stringBuffer, "Invalid element \"%s\" in file \"%s\".", name.c_str(), filePath );
        strcat( stringBuffer, "Valid Required Elements:\n" );
        for( auto iter = required.begin(); iter != required.end(); ++iter )
        {
            strcat( stringBuffer, ( std::string( "    " ) + iter->first + std::string( "\n" ) ).c_str() );
        }
        strcat( stringBuffer, "Valid Optional Elements:\n" );

        for( auto iter = optional.begin(); iter != optional.end(); ++iter )
        {
            strcat( stringBuffer, ( std::string( "    " ) + *iter + std::string( "\n" ) ).c_str() );
        }
        MessageBoxA( NULL, stringBuffer, "Invalid Element", MB_OK | MB_ICONERROR | MB_TOPMOST );

        __debugbreak();
        exit( 1 );
    }

    void XMLReader::errorInvalidAttrib( const TiXmlElement& parent, const std::string& name, int row, const std::map< std::string, bool >& required, const std::vector< std::string >& optional )
    {
        char filePath[250];
        char stringBuffer[250];
        std::string error = "Invalid attribute \"" + name + "\"";

        sprintf( filePath, "%s\\%s (%i) : %s\n", getCurrentDirectory().c_str(), parent.GetDocument()->Value(), row, error.c_str() );

        OutputDebugStringA( filePath );
        sprintf( stringBuffer, "Invalid attribute \"%s\" in file \"%s\".", name.c_str(), filePath );
        strcat( stringBuffer, "Valid Required Attributes:\n" );
        for( auto iter = required.begin(); iter != required.end(); ++iter )
        {
            strcat( stringBuffer, ( std::string( "    " ) + iter->first + std::string( "\n" ) ).c_str() );
        }
        strcat( stringBuffer, "Valid Optional Attributes:\n" );

        for( auto iter = optional.begin(); iter != optional.end(); ++iter )
        {
            strcat( stringBuffer, ( std::string( "    " ) + *iter + std::string( "\n" ) ).c_str() );
        }
        MessageBoxA( NULL, stringBuffer, "Invalid Attribute", MB_OK | MB_ICONERROR | MB_TOPMOST );

        __debugbreak();
        exit( 1 );
    }

    void XMLReader::errorCaseMismatchElement( const TiXmlElement& parent, const std::string& name, int row, const std::string& caseCorrectName )
    {
        char filePath[250];
        char stringBuffer[250];
        std::string error = "Case-incorrect element \"" + name + "\"";

        sprintf( filePath, "%s\\%s (%i) : %s\n", getCurrentDirectory().c_str(), parent.GetDocument()->Value(), row, error.c_str() );

        OutputDebugStringA( filePath );
        sprintf( stringBuffer, "Invalid element \"%s\" in file \"%s\".", name.c_str(), filePath );
        strcat( stringBuffer, (std::string( "Did you mean \"" ) + caseCorrectName + std::string( "\"?\n" )).c_str() );
        MessageBoxA( NULL, stringBuffer, "Case-incorrect Element", MB_OK | MB_ICONERROR | MB_TOPMOST );

        __debugbreak();
        exit( 1 );
    }

    void XMLReader::errorCaseMismatchAttrib( const TiXmlElement& parent, const std::string& name, int row, const std::string& caseCorrectName )
    {
        char filePath[250];
        char stringBuffer[250];
        std::string error = "Case-incorrect attribute \"" + name + "\"";

        sprintf( filePath, "%s\\%s (%i) : %s\n", getCurrentDirectory().c_str(), parent.GetDocument()->Value(), row, error.c_str() );

        OutputDebugStringA( filePath );
        sprintf( stringBuffer, "Invalid attribute \"%s\" in file \"%s\".", name.c_str(), filePath );
        strcat( stringBuffer, (std::string( "Did you mean \"" ) + caseCorrectName + std::string( "\"?\n" )).c_str() );
        MessageBoxA( NULL, stringBuffer, "Case-incorrect Attribute", MB_OK | MB_ICONERROR | MB_TOPMOST );

        __debugbreak();
        exit( 1 );
    }
}