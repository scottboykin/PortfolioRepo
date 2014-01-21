#pragma once

#include <math.h>

namespace SBMath
{
    class vec3
    {

    public:

        float x, y, z;

        explicit vec3( float init = 0 )
        {
            x = y = z = init;
        }

        vec3( float x, float y, float z )
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        float operator[]( int i ) const { return ( &x )[i]; }
        float& operator[]( int i ) { return ( &x )[i]; }
        vec3 operator+( const vec3& other ) const { return vec3( x + other.x, y + other.y, z + other.z ); }
        vec3 operator-( const vec3& other ) const { return vec3( x - other.x, y - other.y, z - other.z ); }
        vec3 operator*( const vec3& other ) const { return vec3( x * other.x, y * other.y, z * other.z ); }
        vec3 operator/( const vec3& other ) const { return vec3( x / other.x, y / other.y, z / other.z ); }
        vec3 operator*( float constant ) const { return vec3( x * constant, y * constant, z * constant ); }
        vec3 operator/( float constant ) const { return vec3( x / constant, y / constant, z / constant ); }
        vec3 operator-() const { return vec3( -x, -y, -z ); }

        vec3& operator+=( const vec3& other )
        { 
            x += other.x;
            y += other.y;
            z += other.z;

            return *this; 
        }

        vec3& operator-=( const vec3& other )
        { 
            x -= other.x;
            y -= other.y;
            z -= other.z;

            return *this;
        }

        vec3& operator*=( float constant )
        {
            x *= constant;
            y *= constant;
            z *= constant;

            return *this;
        }

        vec3& operator/=( float constant )
        {
            constant = 1.0f / constant;

            x *= constant;
            y *= constant;
            z *= constant;

            return *this;
        }

        bool operator==( const vec3& other ) const
        {
            return x == other.x && y == other.y && z == other.z;
        }

        bool operator!=( const vec3& other ) const
        {
            return x != other.x || y != other.y || z != other.z;
        }

        vec3& normalize()
        {
            float distance = sqrt( x * x + y * y + z * z );

            if( distance != 0 )
            {
                float distanceInverse = 1.0f / distance;
                x = x * distanceInverse;
                y = y * distanceInverse;
                z = z * distanceInverse;
            }

            return *this;
        }

        vec3 crossproduct( const vec3& other ) const
        {
            return vec3( 
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x 
                );
        }

        float dotproduct( const vec3& other ) const
        {
            return x * other.x + y * other.y + z * other.z; 
        }

        float squaredDistance( const vec3& other ) const
        {
            return (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y) + (z - other.z) * (z - other.z);
        }
    };
}