#pragma once

#include <math.h>

namespace SBMath
{
    const float PI = 3.141592654f;

    inline float degreeToRadians( float degrees )
    {
        return degrees / 180.0f * PI;
    }

    inline float radiansToDegrees( float radians )
    {
        return radians / PI * 180.0f;
    }

    template < typename dataType >
    dataType clamp( dataType value, dataType min, dataType max )
    {
        if( value < min )
            return min;
        else if( value > max )
            return max;
        else
            return value;
    }

    /*int randInRange( int min, int maxExclusive )
    {
        return min + rand() %( maxExclusive - min );
    }

    float randInRange( float min, float maxInclusive )
    {
        return min + ( maxInclusive - min ) * static_cast< float >( rand() ) / RAND_MAX;
    }*/
}