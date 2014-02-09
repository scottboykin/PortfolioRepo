#include "DebugBox.h"
#include "Renderer.h"
#include "Material.h"
#include "Camera.h"
#include "mat4x4.h"

using namespace SBMath;

namespace SBGraphics
{
    DebugBox::DebugBox( SBMath::vec3& position, SBMath::vec3& axis, float size, float timetoDie )
        :   m_position( position )
        ,   m_axis( axis )
        ,   m_size( size )
    {
        if( m_axis == vec3() )
        {
            m_axis[1] = 1.0;
        }

        m_timeToDie = timetoDie;
        setupVertices();
    }

    void DebugBox::draw()
    {
        glLineWidth( 3.0 );

        Renderer& renderer = Renderer::getRenderer();
        Material& material = renderer.getMaterialManager().getMaterial( "basic" );

        renderer.modelMatrixStack.pushMatrix();

        mat4x4 modelMat;
        modelMat.axisAlignBillboard( Camera::getStaticCamera().position(), m_position, m_axis );

        renderer.modelMatrixStack.loadMatrix( modelMat );
        renderer.modelMatrixStack.scale( m_size, m_size, m_size );

        int useTexture = 0;
        material.setUniform( std::string( "useTexture" ), Material::Uniformi, &useTexture );
        material.setUniform( std::string( "Projection" ), Material::UniformMatrix4f, renderer.projectionMatrixStack.getTop().data() );
        material.setUniform( std::string( "View" ), Material::UniformMatrix4f, renderer.viewMatrixStack.getTop().data() );
        material.setUniform( std::string( "Model" ), Material::UniformMatrix4f, renderer.modelMatrixStack.getTop().data() );

        material.setVertexAttribPointer( std::string( "vertex" ), 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (float*)m_vertices.data() );
        material.setVertexAttribPointer( std::string( "inColor" ), 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (float*)m_vertices.data() + 8 );

        glEnable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        glBlendFunc( GL_SRC_COLOR, GL_ZERO );

        renderer.drawVertexArray( material, GL_TRIANGLES, 0, m_vertices.size() );

        material.setVertexAttribPointer( std::string( "vertex" ), 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (float*)m_vertices.data() );
        material.setVertexAttribPointer( std::string( "inColor" ), 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (float*)m_vertices.data() + 8 );

        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        renderer.drawVertexArray( material, GL_TRIANGLES, 0, m_vertices.size() );

        glDisable( GL_BLEND );
        glEnable( GL_DEPTH_TEST );

        renderer.modelMatrixStack.popMatrix();
    }

    void DebugBox::setupVertices()
    {
        float alpha = 0.25f;
        //Top
        m_vertices.push_back( Vertex( vec3( -0.5f, 0.5f, -0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( 0.5f, 0.5f, -0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( -0.5f, 0.5f, 0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );

        m_vertices.push_back( Vertex( vec3( -0.5f, 0.5f, 0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( 0.5f, 0.5f, -0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( 0.5f, 0.5f, 0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );

        //Bottom
        m_vertices.push_back( Vertex( vec3( -0.5f, -0.5f, -0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( 0.5f, -0.5f, -0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( -0.5f, -0.5f, 0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );

        m_vertices.push_back( Vertex( vec3( -0.5f, -0.5f, 0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( 0.5f, -0.5f, -0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( 0.5f, -0.5f, 0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );

        //Front
        m_vertices.push_back( Vertex( vec3( -0.5f, 0.5f, 0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( 0.5f, 0.5f, 0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( -0.5f, -0.5f, 0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );

        m_vertices.push_back( Vertex( vec3( -0.5f, -0.5f, 0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( 0.5f, 0.5f, 0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( 0.5f, -0.5f, 0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );

        //Back
        m_vertices.push_back( Vertex( vec3( -0.5f, 0.5f, -0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( 0.5f, 0.5f, -0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( -0.5f, -0.5f, -0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );

        m_vertices.push_back( Vertex( vec3( -0.5f, -0.5f, -0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( 0.5f, 0.5f, -0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( 0.5f, -0.5f, -0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );

        //Left
        m_vertices.push_back( Vertex( vec3( -0.5f, 0.5f, -0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( -0.5f, 0.5f, 0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( -0.5f, -0.5f, -0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );

        m_vertices.push_back( Vertex( vec3( -0.5f, -0.5f, -0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( -0.5f, 0.5f, 0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( -0.5f, -0.5f, 0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );

        //Right
        m_vertices.push_back( Vertex( vec3( 0.5f, 0.5f, -0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( 0.5f, 0.5f, 0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha) ) );
        m_vertices.push_back( Vertex( vec3( 0.5f, -0.5f, -0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );

        m_vertices.push_back( Vertex( vec3( 0.5f, -0.5f, -0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( 0.5f, 0.5f, 0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
        m_vertices.push_back( Vertex( vec3( 0.5f, -0.5f, 0.5f ), vec2(), vec4( 1.0, 1.0, 1.0, alpha ) ) );
    }
}