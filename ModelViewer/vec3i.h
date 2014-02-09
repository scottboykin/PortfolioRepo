#pragma once

#include <math.h>

namespace SBMath
{
    class vec3i
    {
    public:
        int x, y, z;

        explicit vec3i( int init = 0 )
        {
            x = y = z = init;
        }

        vec3i( int x, int y, int z )
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        int operator[]( int i ) const { return (&x)[i]; }
        int& operator[]( int i ) { return (&x)[i]; }
        vec3i operator+( vec3i& other ) const { return vec3i( x + other.x, y + other.y, z + other.z ); }
        vec3i operator-( vec3i& other ) const { return vec3i( x - other.x, y - other.y, z - other.z ); }
        vec3i operator*( int constant ) const { return vec3i( x * constant, y * constant, z * constant ); }
        vec3i operator/( int constant ) const { return vec3i( x / constant, y / constant, z / constant ); }
        vec3i operator-() const { return vec3i( -x, -y, -z ); }

        vec3i& operator+=( vec3i& other )
        { 
            x += other.x;
            y += other.y;
            z += other.z;

            return *this; 
        }

        vec3i& operator-=( vec3i& other )
        { 
            x -= other.x;
            y -= other.y;
            z -= other.z;

            return *this;
        }

        vec3i& operator*=( int constant )
        {
            x *= constant;
            y *= constant;
            z *= constant;

            return *this;
        }

        vec3i& operator/=( int constant )
        {
            x /= constant;
            y /= constant;
            z /= constant;

            return *this;
        }

        bool operator==( vec3i& other )
        {
            return x == other.x && y == other.y && z == other.z;
        }

        bool operator!=( vec3i& other )
        {
            return x != other.x || y != other.y || z != other.z;
        }

        vec3i& normalize()
        {
            int distance = (int)sqrt( (float)( x * x + y * y + z * z ) );

            if( distance != 0 )
            {
                x = x / distance;
                y = y / distance;
                z = z / distance;
            }

            return *this;
        }

        vec3i crossproduct( vec3i& other ) const
        {
            return vec3i( 
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x 
                );
        }

        int dotproduct( vec3i& other ) const
        {
            return x * other.x + y * other.y + z * other.z ; 
        }

        int squaredDistance( vec3i& other ) const
        {
            return (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y) + (z - other.z) * (z - other.z);
        }
    };
}