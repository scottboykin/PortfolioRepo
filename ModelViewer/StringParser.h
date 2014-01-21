#pragma once

#include <string>

namespace SBUtil
{
    class StringParser
    {

    public:

        StringParser( std::string& string_to_be_parsed );
        StringParser( std::string& string_to_be_parsed, std::string& delimiters );
        
        std::string getNextToken();
        bool stringContainsChar( std::string& string, char character );
        bool stringFullyParsed(){ return m_posInString >= m_stringToParse.length(); }

        static std::string trim( std::string& string );
        static std::string toLower( std::string& string );
        static std::string toLower( const char* string );

    private:

        std::string m_stringToParse;
        std::string m_delimiters;

        size_t m_posInString;
    };
}