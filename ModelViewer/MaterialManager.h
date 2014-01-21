#pragma once

#include <glew.h>
#include <glut.h>
#include <map>

namespace SBGraphics
{
    class Material;

    class MaterialManager
    {
    public:

        struct Program
        {
            unsigned int vertexShaderID;
            unsigned int fragmentShaderID;
        };

        MaterialManager();
        ~MaterialManager();

        void createMaterial( const char* name, unsigned int programID );
        unsigned int createOrGetProgram( unsigned int vertexShaderID, unsigned int fragmentShaderID );
        unsigned int createOrGetShader( const char* filePath, GLenum shaderType );
        
        void loadTextFile( const char* fileName, GLchar** source );

        Material& getMaterial( const char* name ){ return *( m_materials[name] ); }

    private:

        std::map< unsigned int, Program > m_programs;
        std::map< const char*, unsigned int > m_shaders;
        std::map< const char*, Material* > m_materials;

    };
}