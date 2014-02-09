#pragma once

#include <math.h>

namespace SBMath
{
    class vec2
    {
    public:
        float x, y;

        explicit vec2( float init = 0 )
        {
            x = y = init;
        }

        vec2( float x, float y )
        {
            this->x = x;
            this->y = y;
        }

        float operator[]( int i ) const { return (&x)[i]; }
        float& operator[]( int i ) { return (&x)[i]; }
        vec2 operator+( vec2& other ) const { return vec2( x + other.x, y + other.y ); }
        vec2 operator-( vec2& other ) const { return vec2( x - other.x, y - other.y ); }
        vec2 operator*( float constant ) const { return vec2( x * constant, y * constant ); }
        vec2 operator/( float constant ) const { return vec2( x / constant, y / constant ); }
        vec2 operator-() const { return vec2( -x, -y ); }

        vec2& operator+=( vec2& other )
        { 
            x += other.x;
            y += other.y;

            return *this; 
        }

        vec2& operator-=( vec2& other )
        { 
            x -= other.x;
            y -= other.y;

            return *this;
        }

        vec2& operator*=( float constant )
        {
            x *= constant;
            y *= constant;

            return *this;
        }

        vec2& operator/=( float constant )
        {
            x /= constant;
            y /= constant;

            return *this;
        }

        bool operator==( vec2& other )
        {
            return x == other.x && y == other.y;
        }

        bool operator!=( vec2& other )
        {
            return x != other.x || y != other.y;
        }


        vec2& normalize()
        {
            float distance = sqrt( x * x + y * y );

            if( distance != 0 )
            {
                x = x / distance;
                y = y / distance;
            }

            return *this;
        }

        float dotproduct( vec2& other )
        {
            return x * other.x + y * other.y; 
        }

        float squaredDistance( vec2& other )
        {
            return (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y);
        }
    };
}