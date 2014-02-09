#include "Material.h"
#include <iostream>

namespace SBGraphics
{
    Material::Material( GLuint m_programID )
        :   m_programID( m_programID )
    {}

    void Material::apply()
    {
        glUseProgram( m_programID );
        applyVertexAttribs();
        applyUniforms();
    }

    void Material::remove()
    {
        for( auto iter = m_vertexAttrib.begin(); iter != m_vertexAttrib.end(); ++iter )
        {
            glDisableVertexAttribArray( iter->second.location );
        }
    }

    void Material::setUniform( std::string& name, UniformType uniformType, const GLvoid* pointer, GLint size, int count )
    {
        auto iter = m_uniforms.find( name );
        if( iter == m_uniforms.end() )
        {
            GLint location = glGetUniformLocation( m_programID, name.c_str() );
            if( location != -1 )
            {
                Uniform uniform;

                uniform.location = location;
                uniform.uniformType = uniformType;
                uniform.size = size;
                uniform.count = count;

                if( uniformType != Texture )
                    uniform.pointer = pointer;
                else
                {
                    uniform.pointer = new unsigned int[1];
                    *(unsigned int*)uniform.pointer = *(unsigned int*)pointer; 
                }

                m_uniforms.insert( std::pair< std::string, Uniform >( name, uniform ) );
            }
        }
        else
        {
            Uniform& uniform = iter->second;

            uniform.uniformType = uniformType;
            uniform.size = size;
            uniform.count = count;

            if( uniformType != Texture )
                uniform.pointer = pointer;
            else
            {
                *(unsigned int*)uniform.pointer = *(unsigned int*)pointer; 
            }
        }
    }

    void Material::setVertexAttribPointer( std::string& name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer )
    {
        auto iter = m_vertexAttrib.find( name );
        if( iter == m_vertexAttrib.end() )
        {
            GLint location = glGetAttribLocation( m_programID, name.c_str() );
            if( location != -1 )
            {
                VertexAttrib vertexAttrib;

                vertexAttrib.location = location;
                vertexAttrib.size = size;
                vertexAttrib.type = type;
                vertexAttrib.normalized = normalized;
                vertexAttrib.stride = stride;
                vertexAttrib.pointer = pointer;

                m_vertexAttrib.insert( std::pair< std::string, VertexAttrib >( name, vertexAttrib ) );
            }
        }
        else
        {
            VertexAttrib& vertexAttrib = iter->second;

            vertexAttrib.size = size;
            vertexAttrib.type = type;
            vertexAttrib.normalized = normalized;
            vertexAttrib.stride = stride;
            vertexAttrib.pointer = pointer;
        }
    }

    void Material::setTexture( std::string& name, GLenum target, GLuint textureID )
    {
        setUniform( name, Texture, &textureID, target );
    }

    void Material::applyVertexAttribs()
    {
        for( auto iter = m_vertexAttrib.begin(); iter != m_vertexAttrib.end(); ++iter )
        {
            VertexAttrib& currentVertexAttrib = iter->second;

            glEnableVertexAttribArray( iter->second.location );
            glVertexAttribPointer
                ( 
                iter->second.location, 
                currentVertexAttrib.size,
                currentVertexAttrib.type,
                currentVertexAttrib.normalized,
                currentVertexAttrib.stride,
                currentVertexAttrib.pointer
                );
        }
    }

    void Material::applyUniforms()
    {
        for( auto iter = m_uniforms.begin(); iter != m_uniforms.end(); ++iter )
        {
            Uniform& uniform = iter->second;

            switch( uniform.uniformType )
            {
                case Uniformi: 
                    applyUniformi( uniform );
                    break;

                case Uniformui:
                    applyUniformui( uniform );
                    break;

                case Uniformf:
                    applyUniformf( uniform );
                    break;

                case UniformMatrix3f:
                    glUniformMatrix3fv( uniform.location, uniform.count, GL_FALSE, (float*)uniform.pointer );
                    break;

                case UniformMatrix4f:
                    glUniformMatrix4fv( uniform.location, uniform.count, GL_FALSE, (float*)uniform.pointer );
                    break;

                case Texture:
                    glActiveTexture( uniform.size );
                    GLuint texID = *( (unsigned int*)uniform.pointer );
                    glBindTexture( GL_TEXTURE_2D, *( (unsigned int*)uniform.pointer ) );
                    glUniform1i( uniform.location, uniform.size - GL_TEXTURE0 );
            }
        }
    }


    void Material::applyUniformi( Uniform& uniform )
    {
        switch( uniform.size )
        {
            case 1: 
                glUniform1iv( uniform.location, uniform.count, (int*)uniform.pointer );
                break;
            case 2: 
                glUniform2iv( uniform.location, uniform.count, (int*)uniform.pointer );
                break;
            case 3: 
                glUniform3iv( uniform.location, uniform.count, (int*)uniform.pointer );
                break;
            case 4: 
                glUniform4iv( uniform.location, uniform.count, (int*)uniform.pointer );
                break;
        }
    }

    void Material::applyUniformui( Uniform& uniform )
    {
        switch( uniform.size )
        {
        case 1: 
            glUniform1uiv( uniform.location, uniform.count, (unsigned int*)uniform.pointer );
            break;
        case 2: 
            glUniform2uiv( uniform.location, uniform.count, (unsigned int*)uniform.pointer );
            break;
        case 3: 
            glUniform3uiv( uniform.location, uniform.count, (unsigned int*)uniform.pointer );
            break;
        case 4: 
            glUniform4uiv( uniform.location, uniform.count, (unsigned int*)uniform.pointer );
            break;
        }
    }

    void Material::applyUniformf( Uniform& uniform )
    {
        switch( uniform.size )
        {
        case 1: 
            glUniform1fv( uniform.location, uniform.count, (float*)uniform.pointer );
            break;
        case 2: 
            glUniform2fv( uniform.location, uniform.count, (float*)uniform.pointer );
            break;
        case 3: 
            glUniform3fv( uniform.location, uniform.count, (float*)uniform.pointer );
            break;
        case 4: 
            glUniform4fv( uniform.location, uniform.count, (float*)uniform.pointer );
            break;
        }
    }
}