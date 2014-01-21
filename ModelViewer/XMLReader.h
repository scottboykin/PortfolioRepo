#pragma once

#include <map>
#include <vector>
#include <string>
#include "Color.h"
#include "vec2.h"
#include "vec2i.h"
#include "vec3.h"
#include "vec3i.h"
#include "tinyxml.h"

namespace SBUtil
{
    typedef TiXmlNode XMLNode;

    class XMLReader
    {

    public:

        XMLReader();
        ~XMLReader();

        TiXmlDocument loadXMLDocument( std::string filePath );

        float getXMLAttributeAsFloat( const TiXmlElement& element, const char* attributeName, float defaultValueIfAttributeNotFound );
        int getXMLAttributeAsInt( const TiXmlElement& element, const char* attributeName, int defaultValueIfAttributeNotFound );
        bool getXMLAttributeAsBool( const TiXmlElement& element, const char* attributeName, bool defaultValueIfAttributeNotFound );
        SBGraphics::Color getXMLAttributeAsColor( const TiXmlElement& element, const char* attributeName, SBGraphics::Color defaultValueIfAttributeNotFound );
        SBMath::vec2 getXMLAttributeAsVector2( const TiXmlElement& element, const char* attributeName, SBMath::vec2 defaultValueIfAttributeNotFound );
        SBMath::vec3 getXMLAttributeAsVector3( const TiXmlElement& element, const char* attributeName, SBMath::vec3 defaultValueIfAttributeNotFound );
        SBMath::vec2i getXMLAttributeAsIntVector2( const TiXmlElement& element, const char* attributeName, SBMath::vec2i defaultValueIfAttributeNotFound );
        SBMath::vec3i getXMLAttributeAsIntVector3( const TiXmlElement& element, const char* attributeName, SBMath::vec3i defaultValueIfAttributeNotFound );
        std::string getXMLAttributeAsString( const TiXmlElement& element, const char* attributeName, std::string defaultValueIfAttributeNotFound );
        std::string getXMLElementPCDataAsString( const TiXmlElement& element );

        void validateXMLChildElements( const TiXmlElement& element, const char* commaSeparatedListOfRequiredChildren, const char* commaSeparatedListOfOptionalChildren );
        void validateXMLAttributes( const TiXmlElement& element, const char* commaSeparatedListOfRequiredAttributes, const char* commaSeparatedListOfOptionalAttributes );

        
    private:

        enum ErrorType
        {
            MISSING_REQUIRED_ELEMENT,
            INVALID_ELEMENT,
            INCORRECT_CASE_ELEMENT,
            MISSING_REQUIRED_ATTRIB,
            INVALID_ATTRIB,
            INCORRECT_CASE_ATTRIB
        };

        void errorMissingRequiedElement( const TiXmlElement& parent, const std::string& name, int row );
        void errorMissingRequiedAttrib( const TiXmlElement& parent, const std::string& name, int row );
        void errorInvalidElement( const TiXmlElement& parent, const std::string& name, int row, const std::map< std::string, bool >& required, const std::vector< std::string >& optional );
        void errorInvalidAttrib( const TiXmlElement& parent, const std::string& name, int row, const std::map< std::string, bool >& required, const std::vector< std::string >& optional );
        void errorCaseMismatchElement( const TiXmlElement& parent, const std::string& name, int row, const std::string& caseCorrectName );
        void errorCaseMismatchAttrib( const TiXmlElement& parent, const std::string& name, int row, const std::string& caseCorrectName );
    };
}