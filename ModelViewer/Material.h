#pragma once

#include <map>
#include <string>
#include <glew.h>
#include <glut.h>

namespace SBGraphics
{
    class Material
    {
    public:

        enum UniformType
        {
            Uniformi,
            Uniformui,
            Uniformf,
            UniformMatrix3f,
            UniformMatrix4f,
            Texture
        };

        struct VertexAttrib
        {
            GLint location;
            GLint size;
            GLenum type;
            GLboolean normalized;
            GLsizei stride;
            const GLvoid* pointer;
        };

        struct Uniform
        {
            GLint location;
            GLint size;         //used for the GL_TEXTURE location in Texture uniforms
            int count;
            UniformType uniformType;
            const GLvoid* pointer;
        };

        Material( GLuint programID );

        void apply();
        void remove();
        
        void setUniform( std::string& name, UniformType type, const GLvoid* pointer, GLint size = 1, int count = 1 );
        void setVertexAttribPointer( std::string&, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer );
        void setTexture( std::string&, GLenum target, GLuint textureID );

        void applyUniforms();
        void applyVertexAttribs();

        GLuint getProgramID(){ return m_programID; }

    private:

        void applyUniformi( Uniform& uniform );
        void applyUniformui( Uniform& uniform );
        void applyUniformf( Uniform& uniform );
        
        std::map< std::string, Uniform > m_uniforms;
        std::map< std::string, VertexAttrib > m_vertexAttrib;

        GLuint m_programID;
    };
}