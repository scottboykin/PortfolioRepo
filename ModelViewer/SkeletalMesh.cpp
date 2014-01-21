#include <iostream>
#include <fstream>
#include "SkeletalMesh.h"
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

    SkeletalMesh::SkeletalMesh()
    {}

    SkeletalMesh::SkeletalMesh( FILE* data, int numOfVerts )
    {
        readSkeletalMeshData( data, numOfVerts );
    }

    void SkeletalMesh::readSkeletalMeshData( FILE* data, int numOfVerts )
    {
        std::vector< SkinnedVertex > vertices( numOfVerts );

        fread( vertices.data(), sizeof( SkinnedVertex ), numOfVerts, data );

        for( unsigned int i = 0; i < vertices.size(); ++i )
        {
            m_vertices[vertices[i].materialID].push_back( vertices[i] );
        }
    }

    void SkeletalMesh::render( std::vector< Material >& materials, std::map< int, Bone >& bones, unsigned int frame )
    {
        for( auto iter = m_vertices.begin(); iter != m_vertices.end(); ++iter )
        {
            Renderer& renderer = Renderer::getRenderer();
            Material& modelMaterial = materials[iter->first];

            renderer.modelMatrixStack.pushMatrix();

            int isSkinned = 1;
            modelMaterial.setUniform( std::string( "isSkinned" ), Material::Uniformi, &isSkinned );

            for( auto boneIter = bones.begin(); boneIter != bones.end(); ++boneIter )
            {
                char boneTMString[250];
                int currentTransform = frame % boneIter->second.transforms.size();

                sprintf( boneTMString, "bonetm[%i]", boneIter->first );                
                modelMaterial.setUniform( std::string( boneTMString ), Material::UniformMatrix4f, &boneIter->second.getCurrentTransform( frame ) );
            }

            modelMaterial.setUniform( std::string( "Projection" ), Material::UniformMatrix4f, renderer.projectionMatrixStack.getTop().data() );
            modelMaterial.setUniform( std::string( "View" ), Material::UniformMatrix4f, renderer.viewMatrixStack.getTop().data() );
            modelMaterial.setUniform( std::string( "Model" ), Material::UniformMatrix4f, renderer.modelMatrixStack.getTop().data() );

            modelMaterial.setVertexAttribPointer( std::string( "vertex" ), 3, GL_FLOAT, GL_FALSE, sizeof( SkinnedVertex ), (float *)iter->second.data() );
            modelMaterial.setVertexAttribPointer( std::string( "normal" ), 3, GL_FLOAT, GL_FALSE, sizeof( SkinnedVertex ), (float *)iter->second.data() + SkinnedVertex::normalOffset );
            modelMaterial.setVertexAttribPointer( std::string( "texcoord" ), 2, GL_FLOAT, GL_FALSE, sizeof( SkinnedVertex ), (float *)iter->second.data() + SkinnedVertex::texcoordsOffset );
            
            modelMaterial.setVertexAttribPointer( std::string( "boneIndices" ), 4, GL_FLOAT, GL_FALSE, sizeof( SkinnedVertex ), (char*)iter->second.data() + offsetof( SkinnedVertex, boneIndices ) );
            modelMaterial.setVertexAttribPointer( std::string( "boneWeights" ), 4, GL_FLOAT, GL_FALSE, sizeof( SkinnedVertex ), (char*)iter->second.data() + offsetof( SkinnedVertex, boneWeights ) );

            float weight = *(float*)( (char*)iter->second.data() + offsetof( SkinnedVertex, boneWeights )  + sizeof( SkinnedVertex ) * 545 );

            renderer.drawVertexArray( modelMaterial, GL_TRIANGLES, 0, iter->second.size() );

            renderer.modelMatrixStack.popMatrix();
        }
    }
}