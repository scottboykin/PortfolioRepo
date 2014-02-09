#include <iostream>
#include <fstream>
#include "Model.h"
#include "StringParser.h"
#include "vec3.h"
#include "vec2.h"
#include "Texture.h"
#include "Renderer.h"
#include "mat4x4.h"
#include "Matrix3.h"
#include "DebugOutputManager.h"
#include "DebugOutput.h"

using namespace SBUtil;
using namespace SBMath;

namespace SBGraphics
{
    const int DIFFUSE = 1;
    const int SPECULAR = 4;
    const int EMISSIVE = 5;
    const int NORMAL = 8;

    Model::Model()
        :   m_bShowBones( false )
    {}

    Model::Model( const char* filePath, int programID )
        :   m_bShowBones( false )
    {
        loadModel( filePath, programID );
    }

    bool Model::loadModel( const char* filePath, int programID )
    {
        FILE* file = fopen( filePath, "rb" );

        if( !file )
            return false;

        while( !feof( file ) )
        {
            std::string line;
            char buffer = '\0';
            while( !feof( file ) && buffer != '\n' )
            {
                buffer = (char)fgetc( file );
                line += buffer;
            }

            StringParser lineParser( line, std::string( ", " ) );
            std::string type = lineParser.getNextToken();

            if( type.compare( "StaticMesh" ) == 0 )
            {
                Mesh newMesh;
                std::string meshData;
                int numOfVerts = atoi( lineParser.getNextToken().c_str() );
                int numOfTransforms = atoi( lineParser.getNextToken().c_str() );
                int transformID = atoi( lineParser.getNextToken().c_str() );
                int parentTransformID = atoi( lineParser.getNextToken().c_str() );

                newMesh.readMeshData( file, numOfVerts, numOfTransforms, transformID, parentTransformID );

                m_Meshs.insert( std::pair< int, Mesh >( newMesh.getTransformID(), newMesh ) );

            }

            else if( type.compare( "SkeletalMesh" ) == 0 )
            { 
                SkeletalMesh skeletalMesh;

                int numOfVerts = atoi( lineParser.getNextToken().c_str() );

                skeletalMesh.readSkeletalMeshData( file, numOfVerts );

                m_SkeletalMeshs.push_back( skeletalMesh );
            }

            else if( type.compare( "Bone" ) == 0 )
            {
                Bone bone;

                int boneID = atoi( lineParser.getNextToken().c_str() );
                int numOfTransforms = atoi( lineParser.getNextToken().c_str() );

                std::vector< Matrix3 > transforms( numOfTransforms );
                Matrix3 initialInverse;

                fread( &initialInverse, sizeof( Matrix3 ), 1, file );
                fread( transforms.data(), sizeof( Matrix3 ), numOfTransforms, file );

                bone.initialInverse = initialInverse.convertTo4x4Matrix();
                for( int i = 0; i < numOfTransforms; ++i )
                {
                    bone.transforms.push_back( transforms[i].convertTo4x4Matrix() );
                }

                m_Bones[boneID] = bone;
            }

            else if( type.compare( "Material" ) == 0 )
            {
                int materialID = atoi( lineParser.getNextToken().c_str() );
                int numOfTextures = atoi( lineParser.getNextToken().c_str() );

                std::string materialData;
                for( int i = 0; i < numOfTextures; )
                {
                    buffer = (char)fgetc( file );
                    materialData += buffer;
                    if( buffer == '\n' )
                    {
                        ++i;
                    }
                }

                m_Materials.push_back( loadMaterial( materialData, numOfTextures, programID ) );
            }
        }

        if( m_Materials.size() == 0 )
        {
            m_Materials.push_back( Material( programID ) );
        }

        fclose( file );

        std::cout << "Finished loading " << filePath << std::endl;
        return true;
    }

    Material Model::loadMaterial( std::string& data, int numOfTextures, int programID )
    {
        Material material( programID );

        for( unsigned int i = 0; i < data.length(); ++i )
        {
            std::string line;
            while( data[i] != '\n' )
            {
                line += data[i];
                ++i;
            }            

            StringParser lineParser( line, std::string( ", " ) );
            int textureType = atoi( lineParser.getNextToken().c_str() );
            std::string fileName = lineParser.getNextToken();

            std::string filePath( "Models\\Images\\" );

            if( fileName.length() == 0 )
                return material;

            std::string extension = fileName.substr( fileName.length() - 3, 3 );
            if( extension.compare( "dds" ) == 0 )
            {
                convertToPNG( fileName );
            }

            switch( textureType )
            {
            case DIFFUSE:
                material.setTexture( std::string( "diffuseTex" ), GL_TEXTURE0, Texture::CreateOrGetTexture( filePath + fileName )->getTextureID() );
                break;

            case SPECULAR:
                material.setTexture( std::string( "specularTex" ), GL_TEXTURE1, Texture::CreateOrGetTexture( filePath + fileName )->getTextureID() );
                break;

            case EMISSIVE:
                material.setTexture( std::string( "emissiveTex" ), GL_TEXTURE2, Texture::CreateOrGetTexture( filePath + fileName )->getTextureID() );
                break;

            case NORMAL:
                material.setTexture( std::string( "normalTex" ), GL_TEXTURE3, Texture::CreateOrGetTexture( filePath + fileName )->getTextureID() );
                break;
            }
        }

        return material;
    }

    void Model::convertToPNG( std::string& fileNameToConvert )
    {
        int length = fileNameToConvert.length();
        fileNameToConvert[length - 3] = 'p';
        fileNameToConvert[length - 2] = 'n';
        fileNameToConvert[length - 1] = 'g';
    }

    void Model::render( unsigned int frame )
    {
        for( unsigned int i = 0; i < m_Meshs.size(); ++i )
        {
            m_Meshs[i].render( m_Materials, m_Meshs, frame );
        }

        for( unsigned int i = 0; i < m_SkeletalMeshs.size(); ++i )
        {
            m_SkeletalMeshs[i].render( m_Materials, m_Bones, frame );
        }

        if( m_bShowBones)
        {
            DebugOutputManager dom;
            for( auto iter = m_Bones.begin(); iter != m_Bones.end(); ++iter )
            {
                dom.drawDebugPoint( iter->second.transforms[frame%iter->second.transforms.size()].extractTranslationVector(), iter->second.transforms[frame%iter->second.transforms.size()].extractRotationMatrix() );
            }
        }
    }
}