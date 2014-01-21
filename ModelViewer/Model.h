#pragma once

#include <vector>
#include <map>

#include "Material.h"
#include "Mesh.h"
#include "SkeletalMesh.h"
#include "mat4x4.h"
#include "Bone.h"

namespace SBGraphics
{
    class Model
    {

    public:

        Model();
        Model( const char* filePath, int programID );

        bool loadModel( const char* filePath, int programID );
        Material loadMaterial( std::string& data, int numOfTextures, int programID );
        void convertToPNG( std::string& fileNameToConvert );

        std::vector< Material >& getMaterials(){ return m_Materials; }

        void render( unsigned int frame );

    private:

        bool m_bShowBones;

        std::vector< Material > m_Materials;
        std::vector< SkeletalMesh > m_SkeletalMeshs;
        std::map< int, Mesh > m_Meshs;   
        std::map< int, Bone > m_Bones;

    };
}