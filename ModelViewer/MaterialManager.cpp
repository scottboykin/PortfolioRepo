#include <string>
#include <iostream>
#include <fstream>

#include "MaterialManager.h"
#include "Material.h"
#include "Utilities.h"

using namespace SBUtil;

namespace SBGraphics
{
    MaterialManager::MaterialManager()
    {}

    MaterialManager::~MaterialManager()
    {
        for( auto iter = m_shaders.begin(); iter != m_shaders.end(); ++iter )
        {
            glDeleteShader( iter->second );
        }

        for( auto iter = m_programs.begin(); iter != m_programs.end(); ++iter )
        {
            glDeleteProgram( iter->first );
        }

        for( auto iter = m_materials.begin(); iter != m_materials.end(); ++iter )
        {
            delete iter->second;
        }
    }

    void MaterialManager::createMaterial( const char* name, unsigned int programID )
    {
        if( m_materials.find( name ) == m_materials.end() )
        {
            m_materials.insert( std::pair< const char*, Material* >( name, new Material( programID ) ) );
        }
    }

    unsigned int MaterialManager::createOrGetProgram( unsigned int vertexShaderID, unsigned int fragmentShaderID )
    {
        for( auto iter = m_programs.begin(); iter != m_programs.end(); ++iter )
        {
            if( iter->second.vertexShaderID == vertexShaderID && iter->second.fragmentShaderID == fragmentShaderID )
            {
                return iter->first;
            }
        }

        GLuint programID = glCreateProgram();

        glAttachShader( programID, vertexShaderID );
        glAttachShader( programID, fragmentShaderID );

        GLint success;

        glLinkProgram( programID );
        glGetProgramiv( programID, GL_LINK_STATUS, &success );

        if( !success )
        {
            GLint length;
            glGetProgramiv( programID, GL_INFO_LOG_LENGTH, &length );

            GLchar* log = new GLchar[length];
            glGetProgramInfoLog( programID, length, &length, log );

            for( int i = 0; i < length - 1; ++i )
            {
                log[i] = log[i + 1];
            }
            log[length - 1] = 0;


            OutputDebugStringA( log );
            std::string directory = getCurrentDirectory();

            MessageBoxA(NULL, log, "Linking Error", MB_OK);

            exit( 1 );
        }
        else
        {
            Program program;
            program.vertexShaderID = vertexShaderID;
            program.fragmentShaderID = fragmentShaderID;

            m_programs.insert( std::pair< unsigned int, Program >( programID, program ) );

            return programID;
        }
    }

    unsigned int MaterialManager::createOrGetShader( const char* filePath, GLenum shaderType )
    {
        for( auto iter = m_shaders.begin(); iter != m_shaders.end(); ++iter )
        {
            if( strcmp( iter->first, filePath ) == 0 )
            {
                return iter->second;
            }
        }

        GLchar* sourceBuffer;
        loadTextFile( filePath, &sourceBuffer );

        const GLchar* sb = sourceBuffer;

        GLuint shaderID = glCreateShader( shaderType );
        glShaderSource( shaderID, 1, &sb, NULL );
        glCompileShader( shaderID );
        delete[] sourceBuffer;

        GLint success;
        glGetShaderiv( shaderID, GL_COMPILE_STATUS, &success );

        if( !success )
        {
            GLint length;
            glGetShaderiv( shaderID, GL_INFO_LOG_LENGTH, &length );

            GLchar* log = new GLchar[length];
            glGetShaderInfoLog( shaderID, length, &length, log );

            for( int i = 0; i < length - 1; ++i )
            {
                log[i] = log[i + 1];
            }
            log[length - 1] = 0;

            OutputDebugStringA( log );
            std::string directory = getCurrentDirectory();

            OutputDebugStringA( (directory + "\\" + filePath + log).c_str() );

            MessageBoxA(NULL, log, "Compiling Error", MB_OK | MB_ICONERROR );

            exit( 1 );
        }

        else
        {
            m_shaders.insert( std::pair< const char*, unsigned int >( filePath, shaderID ) );
            return shaderID;
        }
    }

    void MaterialManager::loadTextFile( const char* fileName, GLchar** source )
    {
        std::ifstream inputFile( fileName );

        if( inputFile.is_open() )
        {
            unsigned long length = 0;
            if( inputFile.good() )
            {
                inputFile.seekg( 0, std::ios::end );
                length = (unsigned long)inputFile.tellg();
                inputFile.seekg( std::ios::beg );
            }

            if( length != 0 )
            {
                *source = new GLchar[length + 1];
                int i = 0;
                while( !inputFile.eof() )
                {
                    GLchar temp = inputFile.get();
                    *((*source) + i) = temp;
                    ++i;
                }
                *((*source) + i - 1) = 0;
                std::cout << "File Read" << std::endl;

            }
            inputFile.close();
        }
    }
}