#include "Bone.h"

namespace SBGraphics
{
    SBMath::mat4x4& Bone::getCurrentTransform( int frame )
    {
        if( currentFrame != frame )
        {
            currentFrame = frame;
            currentTransform = initialInverse * transforms[frame % transforms.size()];
        }

        return currentTransform;
    }
}