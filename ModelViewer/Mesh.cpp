#include <iostream>
#include <fstream>
#include "Mesh.h"
#include "StringParser.h"
#include "vec3.h"
#include "vec2.h"
#include "Material.h"
#include "Texture.h"
#include "Renderer.h"
#include "mat4x4.h"
#include "Utilities.h"
#include "Matrix3.h"

using namespace SBUtil;
using namespace SBMath;

namespace SBGraphics
{
    const int DIFFUSE = 1;
    const int SPECULAR = 4;
    const int EMISSIVE = 5;
    const int NORMAL = 8;

    Mesh::Mesh()
        :   m_transformID( 0 )
        ,   m_parentTransformID( -1 )
    {}

    Mesh::Mesh( FILE* data, int numOfVerts, int numOfTransforms, int transformID, int parentTransformID )
    {
        readMeshData( data, numOfVerts, numOfTransforms, transformID, parentTransformID );
    }

    void Mesh::readMeshData( FILE* data, int numOfVerts, int numOfTransforms, int transformID, int parentTransformID )
    {
        std::vector< Vertex > vertices( numOfVerts );
        std::vector< Matrix3 > transforms( numOfTransforms );

        fread( vertices.data(), sizeof( Vertex ), numOfVerts, data );
        fread( transforms.data(), sizeof( Matrix3 ), numOfTransforms, data );
        
        for( unsigned int i = 0; i < vertices.size(); ++i )
        {
            m_vertices[vertices[i].materialID].push_back( vertices[i] );
        }

        for( unsigned int i = 0; i < transforms.size(); ++i )
        {
            m_transforms.push_back( transforms[i].convertTo4x4Matrix() );
        }

        m_transformID = transformID;
        m_parentTransformID = parentTransformID;
    }

    void Mesh::render( std::vector< Material >& materials, std::map< int, Mesh >& meshes, unsigned int frame )
    {
        for( auto iter = m_vertices.begin(); iter != m_vertices.end(); ++iter )
        {
            Renderer& renderer = Renderer::getRenderer();
            Material& modelMaterial = materials[iter->first];

            renderer.modelMatrixStack.pushMatrix();

            unsigned int currentTransformationFrame = frame % m_transforms.size(); 

            mat4x4 modelTransform;
            mat4x4 parentModelTransform = m_transforms[currentTransformationFrame];
            int parentID = m_parentTransformID;

            while( true )
            {
                modelTransform = modelTransform * parentModelTransform;

                if( parentID == -1 )
                {
                    break;
                }
                else
                {
                    currentTransformationFrame = frame % meshes[parentID].getTransforms().size();
                    parentModelTransform = meshes[parentID].getTransforms()[currentTransformationFrame];
                    parentID = meshes[parentID].getParentTransformID();
                }
            }

            renderer.modelMatrixStack.preMultipy( modelTransform );

            int isSkinned = 0;
            modelMaterial.setUniform( std::string( "isSkinned" ), Material::Uniformi, &isSkinned );

            modelMaterial.setUniform( std::string( "Projection" ), Material::UniformMatrix4f, renderer.projectionMatrixStack.getTop().data() );
            modelMaterial.setUniform( std::string( "View" ), Material::UniformMatrix4f, renderer.viewMatrixStack.getTop().data() );
            modelMaterial.setUniform( std::string( "Model" ), Material::UniformMatrix4f, renderer.modelMatrixStack.getTop().data() );

            modelMaterial.setVertexAttribPointer( std::string( "vertex" ), 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (float *)iter->second.data() );
            modelMaterial.setVertexAttribPointer( std::string( "normal" ), 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (float *)iter->second.data() + Vertex::normalOffset );
            modelMaterial.setVertexAttribPointer( std::string( "texcoord" ), 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (float *)iter->second.data() + Vertex::texcoordsOffset );

            renderer.drawVertexArray( modelMaterial, GL_TRIANGLES, 0, iter->second.size() );

            renderer.modelMatrixStack.popMatrix();
        }
    }
}