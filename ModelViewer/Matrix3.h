#pragma once

#include "mat4x4.h"

namespace SBGraphics
{
    class Matrix3
    {

    public:

        Matrix3()
        {
            for( float* ptr = &ix; ptr <= &tz; ++ptr )
            {
                *ptr = 0;
            }
        };

        float ix, iy, iz;
        float jx, jy, jz;
        float kx, ky, kz;
        float tx, ty, tz;
        long flags;

        SBMath::mat4x4 convertTo4x4Matrix();
    };
}