#include "DebugPath.h"
#include "Renderer.h"
#include "Material.h"

namespace SBGraphics
{
    DebugPath::DebugPath( SBMath::vec3& startPosition, SBMath::vec3& endPosition, float timeToDie )
        :   m_startPos( startPosition )
        ,   m_endPos( endPosition )
    {
        m_timeToDie = timeToDie;
        m_currentSimTime = 0.0;
        setupVertices();
    }

    void DebugPath::draw()
    {
        glLineWidth( 3.0 );

        Renderer& renderer = Renderer::getRenderer();
        Material& material = renderer.getMaterialManager().getMaterial( "basic" );

        int useTexture = 0;
        material.setUniform( std::string( "useTexture" ), Material::Uniformi, &useTexture );
        material.setUniform( std::string( "Projection" ), Material::UniformMatrix4f, renderer.projectionMatrixStack.getTop().data() );
        material.setUniform( std::string( "View" ), Material::UniformMatrix4f, renderer.viewMatrixStack.getTop().data() );
        material.setUniform( std::string( "Model" ), Material::UniformMatrix4f, renderer.modelMatrixStack.getTop().data() );

        material.setVertexAttribPointer( std::string( "vertex" ), 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 7, m_vertices );
        material.setVertexAttribPointer( std::string( "inColor" ), 4, GL_FLOAT, GL_FALSE, sizeof( float ) * 7, m_vertices + 3 );

        renderer.drawVertexArray( material, GL_LINES, 0, 2 );

        material.setVertexAttribPointer( std::string( "vertex" ), 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 7, m_vertices + 14 );
        material.setVertexAttribPointer( std::string( "inColor" ), 4, GL_FLOAT, GL_FALSE, sizeof( float ) * 7, m_vertices + 17 );

        glEnable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        renderer.drawVertexArray( material, GL_LINES, 0, 2 );

        glDisable( GL_BLEND );
        glEnable( GL_DEPTH_TEST );
    }

    void DebugPath::setupVertices()
    {
        m_vertices[0] = m_startPos.x;
        m_vertices[1] = m_startPos.y;
        m_vertices[2] = m_startPos.z;
        m_vertices[3] = 0.0f;
        m_vertices[4] = 1.0f;
        m_vertices[5] = 0.0f;
        m_vertices[6] = 1.0f;

        m_vertices[7] = m_endPos.x;
        m_vertices[8] = m_endPos.y;
        m_vertices[9] = m_endPos.z;
        m_vertices[10] = 1.0f;
        m_vertices[11] = 0.0f;
        m_vertices[12] = 0.0f;
        m_vertices[13] = 1.0f;

        m_vertices[14] = m_startPos.x;
        m_vertices[15] = m_startPos.y;
        m_vertices[16] = m_startPos.z;
        m_vertices[17] = 0.0f;
        m_vertices[18] = 1.0f;
        m_vertices[19] = 0.0f;
        m_vertices[20] = 0.25f;

        m_vertices[21] = m_endPos.x;
        m_vertices[22] = m_endPos.y;
        m_vertices[23] = m_endPos.z;
        m_vertices[24] = 1.0f;
        m_vertices[25] = 0.0f;
        m_vertices[26] = 0.0f;
        m_vertices[27] = 0.25f;
    }
}