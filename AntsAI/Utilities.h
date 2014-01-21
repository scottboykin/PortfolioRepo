#pragma once

#include <cassert>
#include <string>
#include <sstream>
#include <crtdbg.h>
#include <Windows.h>

#define PREVENT_COPYING( className ) \
    private: \
    className( const className& ); \
    className& operator=( const className ); \

namespace SBUtil
{
    template< typename T > T clamp( T value, T min, T max )
    {
        if( value > max )
            return max;

        else if( value < min )
            return min;

        else
            return value;
    }

    template< typename T > T getMin( T a, T b )
    {
        return a < b ? a : b;
    }

    template< typename T > T getMax( T a, T b )
    {
        return a > b ? a : b;
    }

    template< typename T > T roundUp( T a )
    {
        return (T)( ( (int)a ) + 1 );
    }

    template< typename T > T roundDown( T a )
    {
        return (T)( (int)a );
    }

    const float PI = 3.141592654f;

    inline float degreeToRadians( float degrees )
    {
        return degrees / 180.0f * PI;
    }

    inline float radiansToDegrees( float radians )
    {
        return radians / PI * 180.0f;
    }

    inline float randInRange( float min, float max )
    {
        const float r = rand() / static_cast< float >( RAND_MAX ); 
        return min + ( max - min ) * r;
    }

    inline void getRandomVelocity( float speed, float& velX, float& velY )
    {
        const float initialAngleRadians = randInRange( 0, 2 * PI );
        velX = std::sin( initialAngleRadians ) * speed;
        velY = std::cos( initialAngleRadians ) * speed;
    }
}

#define trace( expr )  \
{   \
    std::ostringstream helper;  \
    helper << expr << std::endl;    \
    SBUtil::TraceMessage( helper.str() );   \
}   