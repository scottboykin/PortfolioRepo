#pragma once

#include <math.h>

namespace SBMath
{
    class vec4i
    {
    public:
        int x, y, z, w;

        explicit vec4i( int init = 0 )
        {
            x = y = z = w = init;
        }

        vec4i( int x, int y, int z, int w )
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }

        int operator[]( int i ) const { return (&x)[i]; }
        int& operator[]( int i ) { return (&x)[i]; }
        vec4i operator+( vec4i& other ) const { return vec4i( x + other.x, y + other.y, z + other.z, w + other.w ); }
        vec4i operator-( vec4i& other ) const { return vec4i( x - other.x, y - other.y, z - other.z, w - other.w ); }
        vec4i operator*( int constant ) const { return vec4i( x * constant, y * constant, z * constant, w * constant ); }
        vec4i operator/( int constant ) const { return vec4i( x / constant, y / constant, z / constant, w / constant ); }
        vec4i operator-() const { return vec4i( -x, -y, -z, -w ); }

        vec4i& operator+=( vec4i& other )
        { 
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;

            return *this; 
        }

        vec4i& operator-=( vec4i& other )
        { 
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;

            return *this;
        }

        vec4i& operator*=( int constant )
        {
            x *= constant;
            y *= constant;
            z *= constant;
            w *= constant;

            return *this;
        }

        vec4i& operator/=( int constant )
        {
            x /= constant;
            y /= constant;
            z /= constant;
            w /= constant;

            return *this;
        }

        bool operator==( vec4i& other )
        {
            return x == other.x && y == other.y && z == other.z && w == other.w;
        }

        bool operator!=( vec4i& other )
        {
            return x != other.x || y != other.y || z != other.z || w != other.w;
        }

        vec4i& normalize()
        {
            int distance = (int)sqrt( (float)( x * x + y * y + z * z + w * w ) );

            if( distance != 0 )
            {
                x = x / distance;
                y = y / distance;
                z = z / distance;
                w = w / distance;
            }

            return *this;
        }

        int dotproduct( vec4i& other ) const
        {
            return x * other.x + y * other.y + z * other.z + w * other.w; 
        }

        int squaredDistance( vec4i& other ) const
        {
            return (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y) + (z - other.z) * (z - other.z) + ( w - other.w ) * ( w - other.w );
        }
    };
}