#include "DebugEntity.h"
#include "Renderer.h"
#include "Material.h"
#include "vec4.h"

using namespace SBMath;

namespace SBGraphics
{
    DebugEntity::DebugEntity( Entity& entity, float timeToDie )
        :   m_entity( entity )
    {
        m_timeToDie = timeToDie;
    }

    void DebugEntity::draw()
    {
        glLineWidth( 3.0 );

        Renderer& renderer = Renderer::getRenderer();
        Material& material = renderer.getMaterialManager().getMaterial( "basic" );

        renderer.modelMatrixStack.pushMatrix();
        renderer.modelMatrixStack.loadIdentity();

        vec3& position = m_entity.getPosition();
        vec3& orientation = m_entity.getOrientation();

        renderer.modelMatrixStack.translate( position.x, position.y, position.z );
        renderer.modelMatrixStack.rotate( orientation.x, orientation.y, orientation.z );

        int numOfVerts = m_entity.getVertices().size();

        std::vector< vec4 > color;
        for( int i = 0; i < numOfVerts; ++i )
        {
            color.push_back( vec4( 0.4f, 0.6f, 0.4f, 0.5 ) );
        }

        int useTexture = 0;
        material.setUniform( std::string( "useTexture" ), Material::Uniformi, &useTexture );
        material.setUniform( std::string( "Projection" ), Material::UniformMatrix4f, renderer.projectionMatrixStack.getTop().data() );
        material.setUniform( std::string( "View" ), Material::UniformMatrix4f, renderer.viewMatrixStack.getTop().data() );
        material.setUniform( std::string( "Model" ), Material::UniformMatrix4f, renderer.modelMatrixStack.getTop().data() );

        material.setVertexAttribPointer( std::string( "vertex" ), 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (float*)m_entity.getVertices().data() );
        material.setVertexAttribPointer( std::string( "inColor" ), 4, GL_FLOAT, GL_FALSE, 0, color.data() );

        glEnable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        glBlendFunc( GL_SRC_COLOR, GL_ZERO );

        renderer.drawVertexArray( material, GL_TRIANGLES, 0, numOfVerts );

        material.setVertexAttribPointer( std::string( "vertex" ), 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (float*)m_entity.getVertices().data() );
        material.setVertexAttribPointer( std::string( "inColor" ), 4, GL_FLOAT, GL_FALSE, 0, color.data() );

        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        renderer.drawVertexArray( material, GL_TRIANGLES, 0, numOfVerts );

        glDisable( GL_BLEND );
        glEnable( GL_DEPTH_TEST );

        renderer.modelMatrixStack.popMatrix();
    }
}