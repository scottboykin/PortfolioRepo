#pragma once

#include <math.h>

namespace SBMath
{
    class vec2i
    {
    public:
        int x, y;

        explicit vec2i( int init = 0 )
        {
            x = y = init;
        }

        vec2i( int x, int y )
        {
            this->x = x;
            this->y = y;
        }

        int operator[]( int i ) const { return (&x)[i]; }
        int& operator[]( int i ) { return (&x)[i]; }
        vec2i operator+( vec2i& other ) const { return vec2i( x + other.x, y + other.y ); }
        vec2i operator-( vec2i& other ) const { return vec2i( x - other.x, y - other.y ); }
        vec2i operator*( int constant ) const { return vec2i( x * constant, y * constant ); }
        vec2i operator/( int constant ) const { return vec2i( x / constant, y / constant ); }
        vec2i operator-() const { return vec2i( -x, -y ); }

        vec2i& operator+=( vec2i& other )
        { 
            x += other.x;
            y += other.y;

            return *this; 
        }

        vec2i& operator-=( vec2i& other )
        { 
            x -= other.x;
            y -= other.y;

            return *this;
        }

        vec2i& operator*=( int constant )
        {
            x *= constant;
            y *= constant;

            return *this;
        }

        vec2i& operator/=( int constant )
        {
            x /= constant;
            y /= constant;

            return *this;
        }

        bool operator==( vec2i& other )
        {
            return x == other.x && y == other.y;
        }

        bool operator!=( vec2i& other )
        {
            return x != other.x || y != other.y;
        }


        vec2i& normalize()
        {
            int distance = (int)sqrt( (float)( x * x + y * y ) );

            if( distance != 0 )
            {
                x = x / distance;
                y = y / distance;
            }

            return *this;
        }

        int dotproduct( vec2i& other )
        {
            return x * other.x + y * other.y; 
        }

        int squaredDistance( vec2i& other )
        {
            return (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y);
        }
    };
}