#include <sstream>

#include "CommandWindow.h"
#include "vec2.h"
#include "Font.h"
#include "Renderer.h"
#include "Material.h"

using namespace SBGraphics;

namespace SBUtil
{
    CommandWindow::CommandWindow( Font font )
        :   m_textSize( 20 )
        ,   displayWindow( false )
        ,   m_consoleFont( font )
        ,   m_prompt( m_consoleFont, vec2( 0.0f, 580.0f ) )
    {
        m_prompt.setCurrentCommandWindow( this );
    }

    void CommandWindow::cprintf( const char* string, vec3& color )
    {
        if( m_log.size() >= 50 )
        {
            m_log.pop_back();
        }

        m_log.push_front( std::pair< std::string, vec3 >( string, color ) );
    }

    void CommandWindow::executeCommand( std::string& command, int argc, std::string* argv )
    {
        auto iter = locateCommand( command );
        if( iter == m_commands.end() )
        {
            std::string error = "ERROR! Command \"" + std::string(command) + "\" does not exist!";
            cprintf( error.c_str(), vec3( 1.0f, 0.0f, 0.0f ) );
        }
        else
        {
            std::string executing = "Executing Command \"" + std::string(command) + "\"...";
            cprintf( executing.c_str(), vec3( 0.0f, 1.0f, 0.0f ) );
            iter->second.first( argc, argv );
        }
    }

    void CommandWindow::registerCommand( const char* commandName, funcP functionPointer, const char* commandDescription )
    {
        auto iter = locateCommand( std::string( commandName ) );
        if( iter != m_commands.end() )
        {
            std::string error = "ERROR! Command \"" + std::string(commandName) + "\" already exist!";
            cprintf( error.c_str(), vec3( 1.0f, 0.0f, 0.0f ) );
        }
        else
        {
            m_commands.insert( std::pair< const char*, std::pair< funcP, const char* > >( 
                commandName, std::pair< funcP, const char* >( functionPointer, commandDescription ) 
                ) );
        }

    }

    void CommandWindow::drawWindow()
    {
        Renderer& renderer = Renderer::getRenderer();
        Material& material = renderer.getMaterialManager().getMaterial( "basic" );

        renderer.projectionMatrixStack.pushMatrix();
        renderer.projectionMatrixStack.loadIdentity();
        renderer.projectionMatrixStack.getTop().orthoProjection( 0.0f, 800.0f, 0.0f, 600.0f, 0.0f, 1.0f );

        renderer.viewMatrixStack.pushMatrix();
        renderer.viewMatrixStack.loadIdentity();

        renderer.modelMatrixStack.pushMatrix();
        renderer.modelMatrixStack.loadIdentity();

        float vertices[] =
        {
            0.0, 0.0, 0.0,
            800.0, 0.0, 0.0,
            0.0, 600.0, 0.0,
            800.0, 600.0, 0.0
        };

        float color[] =
        {
            0.0, 0.0, 0.0, 0.75,
            0.0, 0.0, 0.0, 0.75,
            0.0, 0.0, 0.0, 0.75,
            0.0, 0.0, 0.0, 0.75
        };

        int useTex = 0;
        material.setUniform( std::string( "useTexture" ), Material::Uniformi, &useTex );
        material.setUniform( std::string( "Projection" ), Material::UniformMatrix4f, renderer.projectionMatrixStack.getTop().data() );
        material.setUniform( std::string( "View" ), Material::UniformMatrix4f, renderer.viewMatrixStack.getTop().data() );
        material.setUniform( std::string( "Model" ), Material::UniformMatrix4f, renderer.modelMatrixStack.getTop().data() );

        material.setVertexAttribPointer( std::string( "vertex" ), 3, GL_FLOAT, GL_FALSE, 0, vertices );
        material.setVertexAttribPointer( std::string( "inColor" ), 4, GL_FLOAT, GL_FALSE, 0, color );

        glEnable( GL_BLEND );

        renderer.drawVertexArray( material, GL_TRIANGLE_STRIP, 0, 4 );

        vec2 currentDrawPos( 0, 560 );

        for( unsigned int i = 0; i < m_log.size(); ++i )
        {
            m_consoleFont.drawString( m_log[i].first.c_str(), currentDrawPos, m_log[i].second, m_textSize );
            currentDrawPos[1] -= m_textSize;
        }

        m_prompt.drawCommandPrompt();

        renderer.projectionMatrixStack.popMatrix();
        renderer.viewMatrixStack.popMatrix();
        renderer.modelMatrixStack.popMatrix();

        glDisable( GL_BLEND );
    }

    void CommandWindow::help()
    {
        for( auto iter = m_commands.begin(); iter != m_commands.end(); ++iter )
        {
            std::string commandHelp;
            commandHelp += iter->first + std::string( ": " ) + std::string( iter->second.second );
            cprintf( commandHelp.c_str(), vec3( 1.0f ) );
        }
    }
    
    void CommandWindow::input( unsigned char keyCode )
    {
        m_prompt.commandPromptInput( keyCode );
    }

    std::map< std::string, std::pair< funcP, const char* > >::iterator CommandWindow::locateCommand( std::string& command )
    {
        toLower( command );

        for( std::map< std::string, std::pair< funcP, const char* > >::iterator iter = m_commands.begin(); iter != m_commands.end(); ++iter )
        {
            std::string commandInLowerCase = iter->first;
            toLower( commandInLowerCase );
            
            if( command == commandInLowerCase )
            {
                return iter;
            }
        }
        return m_commands.end();
    }

    void CommandWindow::toLower( std::string& stringToLower )
    {
        size_t strLength = stringToLower.size();
        for( size_t i = 0; i < strLength; ++i )
        {
            stringToLower[i] |= 32;
        }
    }
}