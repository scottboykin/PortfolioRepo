#include <iostream>
#include <vector>

#include "CommandPrompt.h"
#include "CommandWindow.h"
#include "Font.h"
#include "StringParser.h"
#include "Renderer.h"
#include "Material.h"

using namespace SBMath;
using namespace SBGraphics;

namespace SBUtil
{
    const unsigned char ARROW_LEFT = 17;
    const unsigned char ARROW_UP = 18;
    const unsigned char ARROW_RIGHT = 19;
    const unsigned char ARROW_DOWN = 20;
    const unsigned char KEY_DELETE = 127;
    const unsigned char KEY_BACKSPACE = 8;
    const unsigned char KEY_ENTER = 13;
    const unsigned char KEY_HOME = 21;
    const unsigned char KEY_END = 22;

    CommandPrompt::CommandPrompt( Font& promptFont, SBMath::vec2& promptPos )
        :   m_cursorDrawPos( 0.0f, 580.0f )
        ,   m_promptFont( promptFont )
        ,   m_promptPos( promptPos )
        ,   m_cursorPosInPrompt( 0 )
        ,   m_window( 0 )
        ,   m_cursorBlinkTime( 0 )
        ,   m_commandHistoryPos( 0 )
    {
        m_promptWindowPos[0] = vec3( promptPos.x, promptPos.y, 0.0f );
        m_promptWindowPos[1] = vec3( promptPos.x + 800, promptPos.y, 0.0f );
        m_promptWindowPos[2] = vec3( promptPos.x, promptPos.y + 20, 0.0f );
        m_promptWindowPos[3] = vec3( promptPos.x, promptPos.y + 20, 0.0f );
        m_promptWindowPos[4] = vec3( promptPos.x + 800, promptPos.y, 0.0f );
        m_promptWindowPos[5] = vec3( promptPos.x + 800, promptPos.y + 20, 0.0f );
    }

    void CommandPrompt::drawCommandPrompt()
    {
        m_cursorBlinkTime = ( m_cursorBlinkTime + 1 ) % 75;

        Renderer& renderer = Renderer::getRenderer();
        Material& material = renderer.getMaterialManager().getMaterial( "basic" );

        renderer.projectionMatrixStack.pushMatrix();
        renderer.projectionMatrixStack.loadIdentity();
        renderer.projectionMatrixStack.getTop().orthoProjection( 0.0f, 800.0f, 0.0f, 600.0f, 0.0f, 1.0f );

        renderer.viewMatrixStack.pushMatrix();
        renderer.viewMatrixStack.loadIdentity();

        renderer.modelMatrixStack.pushMatrix();
        renderer.modelMatrixStack.loadIdentity();

        int useTex = 0;
        material.setUniform( std::string( "useTexture" ), Material::Uniformi, &useTex );
        material.setUniform( std::string( "Projection" ), Material::UniformMatrix4f, renderer.projectionMatrixStack.getTop().data() );
        material.setUniform( std::string( "View" ), Material::UniformMatrix4f, renderer.viewMatrixStack.getTop().data() );
        material.setUniform( std::string( "Model" ), Material::UniformMatrix4f, renderer.modelMatrixStack.getTop().data() );

        float color[] =
        {
            0.0, 0.0, 0.0, 1.0,
            0.0, 0.0, 0.0, 1.0,
            0.0, 0.0, 0.0, 1.0,
            0.0, 0.0, 0.0, 1.0,
            0.0, 0.0, 0.0, 1.0,
            0.0, 0.0, 0.0, 1.0
        };

        material.setVertexAttribPointer( std::string( "vertex" ), 3, GL_FLOAT, GL_FALSE, 0, m_promptWindowPos );
        material.setVertexAttribPointer( std::string( "inColor" ), 4, GL_FLOAT, GL_FALSE, 0, color );

        renderer.drawVertexArray( material, GL_TRIANGLES, 0, 6 );

        m_promptFont.drawString( ( std::string( "> " ) + m_entry ).c_str(), m_promptPos, vec3( 1.0f, 1.0f, 1.0f ), 20.0f );
        if( m_cursorBlinkTime < 50 )
        {
            vec2 cursorPos( m_promptFont.measureString( ( ">" + m_entry.substr( 0, m_cursorPosInPrompt ) ).c_str(), 20.0f ), 578.0f );
            m_promptFont.drawString( "|", cursorPos, vec3( 1.0f ), 20.0f );
        }

        renderer.projectionMatrixStack.popMatrix();
        renderer.viewMatrixStack.popMatrix();
        renderer.modelMatrixStack.popMatrix();
    }

    void CommandPrompt::commandPromptInput( unsigned char keyCode )
    {
        std::string::iterator posInString;
        switch( keyCode )
        {
            case ARROW_LEFT:
                if( m_cursorPosInPrompt != 0 )
                {
                    --m_cursorPosInPrompt;
                    m_cursorBlinkTime = 0;
                }
                break;

            case ARROW_RIGHT:
                if( m_cursorPosInPrompt != m_entry.length() )
                {
                    ++m_cursorPosInPrompt;
                    m_cursorBlinkTime = 0;
                }
                break;

            case ARROW_UP:
                if( m_commandHistoryPos != m_commandHistory.size() )
                {
                    ++m_commandHistoryPos;
                    m_entry = m_commandHistory[m_commandHistoryPos - 1];
                    m_cursorBlinkTime = 0;
                }
                break;

            case ARROW_DOWN:
                if( m_commandHistoryPos != 0 )
                {
                    --m_commandHistoryPos;
                    if( m_commandHistoryPos == 0 )
                    {
                        m_entry = "";
                    }
                    else
                    {
                        m_entry = m_commandHistory[m_commandHistoryPos - 1];
                    }
                    m_cursorBlinkTime = 0;

                }
                break;

            case KEY_BACKSPACE: //backspace
                if( m_cursorPosInPrompt != 0 )
                {
                    --m_cursorPosInPrompt;
                    posInString = m_entry.begin();
                    posInString += m_cursorPosInPrompt;
                    m_entry.erase( posInString );
                    m_cursorBlinkTime = 0;
                }
                break;

            case KEY_ENTER: //enter
                parseCommand();
                m_commandHistory.push_front( m_entry );
                if( m_commandHistory.size() > 20 )
                {
                    m_commandHistory.pop_back();
                }
                m_entry.clear();
                m_cursorPosInPrompt = 0;
                m_cursorBlinkTime = 0;
                break;

            case KEY_DELETE: //delete key
                posInString = m_entry.begin();
                posInString += m_cursorPosInPrompt;
                m_entry.erase( posInString );
                m_cursorBlinkTime = 0;
                break;

            case KEY_HOME:
                m_cursorPosInPrompt = 0;
                m_cursorBlinkTime = 0;
                break;

            case KEY_END:
                m_cursorPosInPrompt = m_entry.length();
                m_cursorBlinkTime = 0;
                break;

            default: //any other character
                posInString = m_entry.begin();
                posInString += m_cursorPosInPrompt;
                m_entry.insert( posInString, keyCode );
                ++m_cursorPosInPrompt;
                m_cursorBlinkTime = 0;
                break;
        }
    }

    void CommandPrompt::parseCommand()
    {
        if( m_window )
        {
            StringParser parser( m_entry );   
            std::string command = parser.getNextToken();
            int numOfArgs = 0;
            std::vector< std::string > arguments;

            std::string arg;

            while( ( arg = parser.getNextToken() ) != "" )
            {
                ++numOfArgs;
                arguments.push_back( arg );
            }

            m_window->executeCommand( command, numOfArgs, arguments.data() );
        }
    }

    void CommandPrompt::setCurrentCommandWindow( CommandWindow* window )
    {
        m_window = window;
    }
}