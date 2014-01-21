#pragma once

#include <string>
#include <deque>
#include "vec2.h"
#include "vec3.h"

namespace SBGraphics
{
    class Font;
}

namespace SBUtil
{
    class CommandWindow;

    class CommandPrompt
    {
    public:

        CommandPrompt( SBGraphics::Font& promptFont, SBMath::vec2& promptPos );

        void drawCommandPrompt();
        void commandPromptInput( unsigned char keyCode );
        void parseCommand();
        void setCurrentCommandWindow( CommandWindow* window );

    private:

        CommandWindow* m_window;

        SBGraphics::Font& m_promptFont;

        std::string m_entry;

        std::deque< std::string > m_commandHistory;

        size_t m_cursorPosInPrompt;
        size_t m_commandHistoryPos;
        size_t m_cursorBlinkTime;

        SBMath::vec2 m_cursorDrawPos;
        SBMath::vec2 m_promptPos;

        SBMath::vec3 m_promptWindowPos[6];
    };
}