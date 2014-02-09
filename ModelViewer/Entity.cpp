#include "Entity.h"
#include "Renderer.h"
#include "Material.h"
#include "mat4x4.h"
#include "Texture.h"

using namespace SBMath;

namespace SBGraphics
{
    Entity::Entity()
        :   m_position()
        ,   m_orientation()
    {
        setupVertices();
    }

    void Entity::update( float dt )
    {

    }

    void Entity::draw()
    {
        glLineWidth( 3.0 );

        Renderer& renderer = Renderer::getRenderer();
        Material& material = renderer.getMaterialManager().getMaterial( "basic" );

        renderer.modelMatrixStack.pushMatrix();        

        renderer.modelMatrixStack.translate( m_position.x, m_position.y, m_position.z );
        renderer.modelMatrixStack.rotate( m_orientation.x, m_orientation.y, m_orientation.z );

        int useTexture = 1;
        material.setUniform( std::string( "useTexture" ), Material::Uniformi, &useTexture );
        material.setUniform( std::string( "Projection" ), Material::UniformMatrix4f, renderer.projectionMatrixStack.getTop().data() );
        material.setUniform( std::string( "View" ), Material::UniformMatrix4f, renderer.viewMatrixStack.getTop().data() );
        material.setUniform( std::string( "Model" ), Material::UniformMatrix4f, renderer.modelMatrixStack.getTop().data() );
        material.setTexture( std::string( "diffuseTex" ), GL_TEXTURE0, Texture::CreateOrGetTexture( std::string( "cobblestonesDiffuse.png" ) )->getTextureID() );

        material.setVertexAttribPointer( std::string( "vertex" ), 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (float*)m_vertices.data() );
        material.setVertexAttribPointer( std::string( "texcoord" ), 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (float*)m_vertices.data() + Vertex::texcoordsOffset );
        material.setVertexAttribPointer( std::string( "inColor" ), 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (float*)m_vertices.data() + 8 );
        material.setVertexAttribPointer( std::string( "_tangent" ), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (float*)m_vertices.data() + Vertex::tangentOffset );
        material.setVertexAttribPointer( std::string( "_bitangent" ), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (float*)m_vertices.data() + Vertex::bitangentOffset );
        material.setVertexAttribPointer( std::string( "_normal" ), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (float*)m_vertices.data() + Vertex::normalOffset );
        


        renderer.drawElements( material, GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, m_indices.data() );
        //renderer.drawVertexArray( material, GL_TRIANGLES, 0, m_vertices.size() );

        renderer.modelMatrixStack.popMatrix();
    }

    void Entity::setupVertices()
    {
        Renderer::generateCubeVertices( m_vertices, m_indices, true );
        for( unsigned int i = 0; i < m_vertices.size(); ++i )
        {
            m_vertices[i].color = vec4( 1.0, 1.0, 1.0, 1.0 );
        }
        
    }
}