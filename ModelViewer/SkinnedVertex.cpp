#include "SkinnedVertex.h"

using namespace SBMath;

namespace SBGraphics
{
    SkinnedVertex::SkinnedVertex()
        :   materialID( 0 )
    {
        for( int i = 0; i < 4; ++i )
        {
            boneIndices[i] = 0;
            boneWeights[i] = 0;
        }
    }

    SkinnedVertex::SkinnedVertex( vec3& initPosition, vec2& initTexCoords, vec4& initColor )
        :   position( initPosition )
        ,   texcoords( initTexCoords )
        ,   color( initColor )
        ,   materialID( 0 )
    {
        for( int i = 0; i < 4; ++i )
        {
            boneIndices[i] = 0;
            boneWeights[i] = 0;
        }
    }
}