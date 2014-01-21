#include "Matrix3.h"

using namespace SBMath;

namespace SBGraphics
{
    mat4x4 Matrix3::convertTo4x4Matrix()
    {
        float data[] =
        {
            ix, iy, iz, 0,
            jx, jy, jz, 0,
            kx, ky, kz, 0,
            tx, ty, tz, 1
        };

        return mat4x4( data );
    }
}