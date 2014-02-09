#include "Light.h"

namespace SBGraphics
{
    Light::Light()
        :   attenuation( 2 )
        ,   aperture( 0.98 )
        ,   nearFalloff( 1 )
        ,   farFalloff ( 1 )
    {}
}