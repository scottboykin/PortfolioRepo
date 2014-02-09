#pragma once

#include <math.h>

namespace SBMath
{
    class vec4
    {
    public:
        float x, y, z, w;

        explicit vec4( float init = 0 )
        {
            x = y = z = w = init;
        }

        vec4( float x, float y, float z, float w )
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }

        float operator[]( int i ) const { return (&x)[i]; }
        float& operator[]( int i ) { return (&x)[i]; }
        vec4 operator+( vec4& other ) const { return vec4( x + other.x, y + other.y, z + other.z, w + other.w ); }
        vec4 operator-( vec4& other ) const { return vec4( x - other.x, y - other.y, z - other.z, w - other.w ); }
        vec4 operator*( float constant ) const { return vec4( x * constant, y * constant, z * constant, w * constant ); }
        vec4 operator/( float constant ) const { return vec4( x / constant, y / constant, z / constant, w / constant ); }
        vec4 operator-() const { return vec4( -x, -y, -z, -w ); }

        vec4& operator+=( vec4& other )
        { 
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;

            return *this; 
        }

        vec4& operator-=( vec4& other )
        { 
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;

            return *this;
        }

        vec4& operator*=( float constant )
        {
            x *= constant;
            y *= constant;
            z *= constant;
            w *= constant;

            return *this;
        }

        vec4& operator/=( float constant )
        {
            x /= constant;
            y /= constant;
            z /= constant;
            w /= constant;

            return *this;
        }

        bool operator==( vec4& other )
        {
            return x == other.x && y == other.y && z == other.z && w == other.w;
        }

        bool operator!=( vec4& other )
        {
            return x != other.x || y != other.y || z != other.z || w != other.w;
        }

        vec4& normalize()
        {
            float distance = sqrt( x * x + y * y + z * z + w * w );

            if( distance != 0 )
            {
                x = x / distance;
                y = y / distance;
                z = z / distance;
                w = w / distance;
            }

            return *this;
        }

        float dotproduct( vec4& other ) const
        {
            return x * other.x + y * other.y + z * other.z + w * other.w; 
        }

        float squaredDistance( vec4& other ) const
        {
            return (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y) + (z - other.z) * (z - other.z) + ( w - other.w ) * ( w - other.w );
        }
    };
}