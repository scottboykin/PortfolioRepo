#pragma once

#include <vector>
#include <map>
#include <string>
#include "SkinnedVertex.h"
#include "Bone.h"

namespace SBMath
{
    class mat4x4;
}

namespace SBGraphics
{
    class Material;

    class SkeletalMesh
    {

    public:

        SkeletalMesh();
        SkeletalMesh( FILE* data, int numOfVerts );

        void readSkeletalMeshData( FILE* file, int numOfVerts );        

        void render( std::vector< Material >& materials, std::map< int, Bone >& bones, unsigned int frame );

    private:

        std::map< int, std::vector< SkinnedVertex > > m_vertices;

    };
}