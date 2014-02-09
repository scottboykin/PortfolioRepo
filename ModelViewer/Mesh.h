#pragma once

#include <vector>
#include <map>
#include <string>
#include "Vertex.h"

namespace SBMath
{
    class mat4x4;
}

namespace SBGraphics
{
    class Material;

    class Mesh
    {

    public:

        Mesh();
        Mesh( FILE* data, int numOfVerts, int numOfTransforms, int transformID, int parentTransformID );

        void readMeshData( FILE* file, int numOfVerts, int numOfTransforms, int transformID, int parentTransformID );        

        void render( std::vector< Material >& materials, std::map< int, Mesh >& meshes, unsigned int frame );

        int getTransformID(){ return m_transformID; }
        int getParentTransformID(){ return m_parentTransformID; }

        const std::vector< SBMath::mat4x4 > getTransforms(){ return m_transforms; }

    private:

        int m_transformID;
        int m_parentTransformID;
        std::map< int, std::vector< Vertex > > m_vertices;
        std::vector< SBMath::mat4x4 > m_transforms;

    };
}