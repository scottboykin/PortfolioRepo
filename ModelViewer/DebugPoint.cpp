#include "DebugPoint.h"
#include "Renderer.h"
#include "Material.h"
#include "mat4x4.h"

using namespace SBMath;

namespace SBGraphics
{
    DebugPoint::DebugPoint( SBMath::vec3& position, SBMath::mat3x3& orientation, float timeToDie )
        :   m_position( position )
        ,   m_orientation( orientation )
    {
        m_timeToDie = timeToDie;
        setupAxes();
    }

    void DebugPoint::draw()
    {
        glLineWidth( 3.0 );

        Renderer& renderer = Renderer::getRenderer();
        Material& material = renderer.getMaterialManager().getMaterial( "basic" );

        renderer.modelMatrixStack.pushMatrix();
        renderer.modelMatrixStack.loadIdentity();

        float rotationVal[] =
        {
            m_orientation[0], m_orientation[1], m_orientation[2], 0,
            m_orientation[3], m_orientation[4], m_orientation[5], 0,
            m_orientation[6], m_orientation[7], m_orientation[8], 0,
                           0,                0,                0, 1
        };

        float translateVal[] =
        {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            m_position.x, m_position.y, m_position.z, 1
        };

        renderer.modelMatrixStack.loadMatrix( mat4x4( rotationVal ) * mat4x4( translateVal ) );

        int useTexture = 0;
        material.setUniform( std::string( "useTexture" ), Material::Uniformi, &useTexture );
        material.setUniform( std::string( "Projection" ), Material::UniformMatrix4f, renderer.projectionMatrixStack.getTop().data() );
        material.setUniform( std::string( "View" ), Material::UniformMatrix4f, renderer.viewMatrixStack.getTop().data() );
        material.setUniform( std::string( "Model" ), Material::UniformMatrix4f, renderer.modelMatrixStack.getTop().data() );

        material.setVertexAttribPointer( std::string( "vertex" ), 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (float*)m_solidAxes );
        material.setVertexAttribPointer( std::string( "inColor" ), 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (float*)m_solidAxes + Vertex::colorOffset );

        renderer.drawVertexArray( material, GL_LINES, 0, 6 );

        material.setVertexAttribPointer( std::string( "vertex" ), 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (float*)m_translucentAxes );
        material.setVertexAttribPointer( std::string( "inColor" ), 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (float*)m_translucentAxes + Vertex::colorOffset );

        glEnable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        renderer.drawVertexArray( material, GL_LINES, 0, 6 );

        glDisable( GL_BLEND );
        glEnable( GL_DEPTH_TEST );

        renderer.modelMatrixStack.popMatrix();
    }

    void DebugPoint::setupAxes()
    {
        m_solidAxes[0] = Vertex( vec3(), vec2(), vec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        m_solidAxes[1] = Vertex( vec3( 1.0f, 0.0f, 0.0f ), vec2(), vec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        m_solidAxes[2] = Vertex( vec3(), vec2(), vec4( 0.0f, 1.0f, 0.0f, 1.0f ) );
        m_solidAxes[3] = Vertex( vec3( 0.0f, 1.0f, 0.0f ), vec2(), vec4( 0.0f, 1.0f, 0.0f, 1.0f ) );
        m_solidAxes[4] = Vertex( vec3(), vec2(), vec4( 0.0f, 0.0f, 1.0f, 1.0f ) );
        m_solidAxes[5] = Vertex( vec3( 0.0f, 0.0f, 1.0f ), vec2(), vec4( 0.0f, 0.0f, 1.0f, 1.0f ) );

        m_translucentAxes[0] = Vertex( vec3(), vec2(), vec4( 1.0f, 0.0f, 0.0f, 0.5f ) );
        m_translucentAxes[1] = Vertex( vec3( 1.0f, 0.0f, 0.0f ), vec2(), vec4( 1.0f, 0.0f, 0.0f, 0.5f ) );
        m_translucentAxes[2] = Vertex( vec3(), vec2(), vec4( 0.0f, 1.0f, 0.0f, 0.5f ) );
        m_translucentAxes[3] = Vertex( vec3( 0.0f, 1.0f, 0.0f ), vec2(), vec4( 0.0f, 1.0f, 0.0f, 0.5f ) );
        m_translucentAxes[4] = Vertex( vec3(), vec2(), vec4( 0.0f, 0.0f, 1.0f, 0.5f ) );
        m_translucentAxes[5] = Vertex( vec3( 0.0f, 0.0f, 1.0f ), vec2(), vec4( 0.0f, 0.0f, 1.0f, 0.5f ) );
    }
}