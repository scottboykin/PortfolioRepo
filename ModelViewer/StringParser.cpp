#include <iostream>
#include "StringParser.h"

namespace SBUtil
{
    StringParser::StringParser( std::string& string_to_be_parsed )
        :   m_stringToParse( string_to_be_parsed )
        ,   m_delimiters()
        ,   m_posInString( 0 )
    {
        m_delimiters = ' ';
    }

    StringParser::StringParser( std::string& string_to_be_parsed, std::string& delimiters )
        :   m_stringToParse( string_to_be_parsed )
        ,   m_delimiters( delimiters )
        ,   m_posInString( 0 )
    {}

    std::string StringParser::getNextToken()
    {
        if( m_posInString < m_stringToParse.length() )
        {
            int token_start_position = m_posInString;
            while( token_start_position < m_stringToParse.length() && stringContainsChar( m_delimiters, m_stringToParse[token_start_position] ) ) 
            {
                ++token_start_position;
            }
            m_posInString = token_start_position;
            if( m_posInString >= m_stringToParse.size() )
            {
                return "";
            }

            while( m_posInString < m_stringToParse.length() && !stringContainsChar( m_delimiters, m_stringToParse[m_posInString] ) )
            {
                ++m_posInString;
            }
            ++m_posInString;

            std::string stringToReturn = m_stringToParse.substr( token_start_position, m_posInString - 1 - token_start_position );
            return trim( stringToReturn );
        }

        return "";
    }

    bool StringParser::stringContainsChar( std::string& string, char character )
    {
        size_t strLength = string.size();
        for( size_t i = 0; i < strLength; ++i )
        {
            if( string[i] == character )
                return true;
        }

        return false;
    }

    std::string StringParser::trim( std::string& string )
    {
        size_t startPos = 0;
        size_t endPos = string.length() - 1;

        while( string[startPos] == ' ' )
        {
            ++startPos;
        }

        while( string[endPos] == ' ' )
        {
            --endPos;
        }

        if( startPos > endPos )
        {
            return string;
        }

        else
        {
            return string.substr( startPos, endPos + 1 - startPos );
        }
    }

    std::string StringParser::toLower( std::string& string )
    {
        std::string lowerCaseResult = string;

        for( size_t i = 0; i < lowerCaseResult.length(); ++i )
        {
            lowerCaseResult[i] = lowerCaseResult[i] | 32;
        }

        return lowerCaseResult;
    }

    std::string StringParser::toLower( const char* string )
    {
        return toLower( std::string( string ) );
    }
}