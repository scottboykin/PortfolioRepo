#pragma once

#include <deque>
#include <map>
#include <string>
#include "vec3.h"
#include "CommandPrompt.h"
#include "Font.h"

namespace SBUtil
{
    typedef void(*funcP)( int argc, std::string* argv );//change the param to const char* which will contain the args

    class CommandWindow
    {
    public:

        CommandWindow( SBGraphics::Font font );

        void cprintf( const char* string, SBMath::vec3& color );
        void executeCommand( std::string& command, int argc, std::string* argv );
        void registerCommand( const char* commandName, funcP funcPointer, const char* commandDescription );
        void drawWindow();
        void input( unsigned char keyCode );
        void help();

        bool displayWindow;

    private:

        std::map< std::string, std::pair< funcP, const char* > >::iterator locateCommand( std::string& command );
        void toLower( std::string& stringToLower );

        SBGraphics::Font m_consoleFont;
        CommandPrompt m_prompt;

        std::map< std::string, std::pair< funcP, const char* > > m_commands;
        std::deque< std::pair< std::string, SBMath::vec3 > > m_log;
        float m_textSize;

    };
}