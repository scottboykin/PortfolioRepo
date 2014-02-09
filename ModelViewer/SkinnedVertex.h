#pragma once

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

namespace SBGraphics
{
    class SkinnedVertex
    {

    public:

        SkinnedVertex();
        SkinnedVertex( SBMath::vec3& initPosition, SBMath::vec2& initTexCoords  = SBMath::vec2(), SBMath::vec4& initColor = SBMath::vec4( 1.0f ) );

        SBMath::vec3 position;
        SBMath::vec3 normal;
        SBMath::vec2 texcoords;
        SBMath::vec4 color;

        int materialID;

        int boneIndices[4];
        float boneWeights[4];

        //SBMath::vec3 tangent;
        //SBMath::vec3 bitangent;
        //SBMath::vec3 tangentNormal;

        static const int normalOffset = 3;
        static const int texcoordsOffset = 6;
        static const int colorOffset = 8;
        //static const int tangentOffset = 12;
        //static const int bitangentOffset = 15;
        static const int boneIndicesOffset = 13;
        static const int boneWeightsOffset = 17;
    };
}