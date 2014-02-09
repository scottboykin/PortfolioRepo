#pragma once

#include <vector>
#include "mat4x4.h"

namespace SBGraphics
{
    class Bone
    {

    public:

        Bone()
        :   currentFrame( -1 )
        {}

        SBMath::mat4x4& getCurrentTransform( int frame );

        SBMath::mat4x4 initialInverse;
        SBMath::mat4x4 currentTransform;
        std::vector< SBMath::mat4x4 > transforms;
        
        int currentFrame;

    };
}