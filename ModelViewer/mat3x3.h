#pragma once

#include "vec3.h"

namespace SBMath
{
    class mat3x3
    {

    public:

        mat3x3()
        {
            loadIdentity();
        }

        mat3x3( float init )
        {
            for( int i = 0; i < SIZE * SIZE; ++i )
            {
                m_values[i] = init;
            }
        }

        mat3x3( float* values )
        {
            for( int i = 0; i < SIZE * SIZE; ++i)
            {
                m_values[i] = values[i];
            }
        }

        float& operator[]( int i ){ return m_values[i]; }
        mat3x3 operator+( mat3x3& other )
        {
            float newData[SIZE * SIZE];
            for( int i = 0; i < SIZE * SIZE; ++i )
            {
                newData[i] = m_values[i] + other.m_values[i];
            }

            return mat3x3( newData );
        }

        mat3x3 operator-( mat3x3& other )
        {
            float newData[SIZE * SIZE];
            for( int i = 0; i < SIZE * SIZE; ++i )
            {
                newData[i] = m_values[i] - other.m_values[i];
            }

            return mat3x3( newData );
        }

        mat3x3 operator*( float constant )
        {
            float newData[SIZE * SIZE];
            for( int i = 0; i < SIZE * SIZE; ++i )
            {
                newData[i] = m_values[i] * constant;
            }

            return mat3x3( newData );
        }

        mat3x3 operator/( float constant )
        {
            float newData[SIZE * SIZE];
            for( int i = 0; i < SIZE * SIZE; ++i )
            {
                newData[i] = m_values[i] / constant;
            }

            return mat3x3( newData );
        }

        mat3x3 operator*( mat3x3& other )
        {
            float newData[SIZE * SIZE];

            for( int i = 0; i < SIZE * SIZE; ++i )
            {
                newData[i] = 0;
            }

            for( int i = 0; i < SIZE; ++i )
            {
                for( int j = 0; j < SIZE; ++j )
                {
                    for( int k = 0; k < SIZE; ++k )
                    {
                        newData[i * SIZE + j] += m_values[i * SIZE + k] * other.m_values[k * SIZE + j];
                    }
                }
            }

            return mat3x3( newData );
        }

        vec3 operator*( vec3& vec )
        {
            vec3 result;
            for( int i = 0; i < SIZE; ++i )
            {
                for( int j = 0; j < SIZE; ++j )
                {
                    result[i] += m_values[ i * SIZE + j ] * vec[j];
                }
            }

            return result;
        }

        void loadIdentity()
        {
            for( int i = 0; i < SIZE; ++i )
            {
                for( int j = 0; j < SIZE; ++j )
                {
                    if( i == j )
                    {
                        m_values[ i * SIZE + j ] = 1;
                    }
                    else
                    {
                        m_values[ i * SIZE + j ] = 0;
                    }
                }
            }
        }

        void orthonormalize()
        {
            vec3 xAxis( m_values[0], m_values[1], m_values[2] );
            xAxis.normalize();
            
            vec3 yAxis( m_values[3], m_values[4], m_values[5] );

            vec3 zAxis = xAxis.crossproduct( yAxis ).normalize();

            yAxis = zAxis.crossproduct( xAxis );

            m_values[0] = xAxis[0];
            m_values[1] = xAxis[1];
            m_values[2] = xAxis[2];

            m_values[3] = yAxis[0];
            m_values[4] = yAxis[1];
            m_values[5] = yAxis[2];

            m_values[6] = zAxis[0];
            m_values[7] = zAxis[1];
            m_values[8] = zAxis[2];

        }

        mat3x3& fromEulerAngles( float yaw, float pitch, float roll = 0 )
        {
            float fCos, fSin;

            fCos = cos( yaw );
            fSin = sin( yaw );
            float kY[] = 
            {
                fCos, 0.0f, fSin,
                0.0f, 1.0f, 0.0f,
                -fSin, 0.0f, fCos
            };
            mat3x3 kYMat( kY );

            fCos = cos( pitch );
            fSin = sin( pitch );
            float kX[] =
            {
                1.0f , 0.0f, 0.0f,
                0.0f , fCos, -fSin,
                0.0f , fSin, fCos
            };
            mat3x3 kXMat( kX );

            fCos = cos( roll );
            fSin = sin( roll );
            float kZ[] = 
            {
                fCos, -fSin, 0.0f,
                fSin, fCos, 0.0f,
                0.0f, 0.0f, 1.0f
            };
            mat3x3 kZMat( kZ );

            *this = kYMat * kXMat * kZMat;
            return *this;
        }
        

        float* data()
        {
            return m_values;
        }

    private:

        static const int SIZE = 3;

        float m_values[SIZE * SIZE];

    };
}