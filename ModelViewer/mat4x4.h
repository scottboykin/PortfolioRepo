#pragma once

#include "MathHelper.h"
#include "vec3.h"
#include "mat3x3.h"

namespace SBMath
{
    class mat4x4
    {

    public:

        mat4x4()
        {
            loadIdentity();
        }

        explicit mat4x4( float init )
        {
            for( int i = 0; i < SIZE * SIZE; ++i )
            {
                m_values[i] = init;
            }
        }

        explicit mat4x4( float* values )
        {
            for( int i = 0; i < SIZE * SIZE; ++i)
            {
                m_values[i] = values[i];
            }
        }

        float& operator[]( int i ){ return m_values[i]; }
        mat4x4 operator+( mat4x4& other )
        {
            float newData[SIZE * SIZE];
            for( int i = 0; i < SIZE * SIZE; ++i )
            {
                newData[i] = m_values[i] + other.m_values[i];
            }

            return mat4x4( newData );
        }

        mat4x4 operator-( mat4x4& other )
        {
            float newData[SIZE * SIZE];
            for( int i = 0; i < SIZE * SIZE; ++i )
            {
                newData[i] = m_values[i] - other.m_values[i];
            }

            return mat4x4( newData );
        }

        mat4x4 operator*( float constant )
        {
            float newData[SIZE * SIZE];
            for( int i = 0; i < SIZE * SIZE; ++i )
            {
                newData[i] = m_values[i] * constant;
            }

            return mat4x4( newData );
        }

        mat4x4 operator/( float constant )
        {
            float newData[SIZE * SIZE];
            for( int i = 0; i < SIZE * SIZE; ++i )
            {
                newData[i] = m_values[i] / constant;
            }

            return mat4x4( newData );
        }

        mat4x4 operator*( mat4x4& other )
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

            return mat4x4( newData );
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

        void orthoProjection( float left, float right, float top, float bottom, float nearVal, float farVal )
        {
            float tx = -( ( right + left ) / ( right - left ) );
            float ty = -( ( top + bottom ) / ( top - bottom ) );
            float tz = -( ( farVal + nearVal ) / ( farVal - nearVal ) );

            float orthoValues[] =
            {
                2 / ( right - left ), 0, 0, 0,
                0, 2 / ( top - bottom ), 0, 0,
                0, 0, -2 / ( farVal - nearVal ), 0,
                tx, ty, tz, 1
            };

            *this = mat4x4( orthoValues ) * (*this);
        }

        void perspectiveProjection( float yFOV, float aspectRatio, float zNear, float zFar )
        {
            float f = tan( ( PI / 2  ) - degreeToRadians( yFOV / 2 ) );

            float perspectiveValues[]=
            {
                f / aspectRatio, 0, 0, 0,
                0, f, 0, 0,
                0, 0, (zFar + zNear) / ( zNear - zFar), -1,
                0, 0, ( 2 * zFar * zNear ) / ( zNear - zFar ), 0,
            };

            *this = mat4x4( perspectiveValues ) * (*this);
        }

        void translate( float x, float y, float z )
        {
            float translationValues[] =
            {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                   x,    y,    z, 1.0f
            };

            mat4x4 translationMat( translationValues );

            *this = translationMat * (*this);
        }

        void rotate( float yaw, float pitch, float roll )
        {
            float fCos, fSin;

            fCos = cos(yaw);
            fSin = sin(yaw);
            float kY[] = 
            {
                fCos, 0.0f, fSin, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                -fSin, 0.0f, fCos, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };

            mat4x4 kYMat( kY );

            fCos = cos(pitch);
            fSin = sin(pitch);
            float kX[] =
            {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, fCos, -fSin, 0.0f,
                0.0f, fSin, fCos, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };

            mat4x4 kXMat( kX );

            fCos = cos(roll);
            fSin = sin(roll);
            float kZ[] = 
            {
                fCos, -fSin, 0.0f, 0.0f,
                fSin, fCos, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };

            mat4x4 kZMat( kZ );

            *this = kYMat * kXMat * kZMat * (*this);
            
        }

        void scale( float xScale, float yScale, float zScale )
        {
            float scaleValues[] =
            {
                xScale, 0.0f, 0.0f, 0.0f,
                0.0f, yScale, 0.0f, 0.0f,
                0.0f, 0.0f, zScale, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };

            mat4x4 scaleMat( scaleValues );

            *this = scaleMat * (*this);
        }

        mat4x4& worldAlignBillboard( vec3& cameraPos, vec3& billboardPos )
        {
            vec3 zaxis( cameraPos[0] - billboardPos[0], cameraPos[1] - billboardPos[1], cameraPos[2] - billboardPos[2] );
            zaxis.normalize();

            vec3 worldUp( 0.0f, 1.0f, 0.0f );
            vec3 xaxis = worldUp.crossproduct( zaxis );
            xaxis.normalize();

            vec3 yaxis = zaxis.crossproduct( xaxis );
            yaxis.normalize();

            m_values[0] = xaxis[0];
            m_values[1] = xaxis[1];
            m_values[2] = xaxis[2];
            m_values[3] = 0;
            
            m_values[4] = yaxis[0];
            m_values[5] = yaxis[1];
            m_values[6] = yaxis[2];
            m_values[7] = 0;
            
            m_values[8] = zaxis[0];
            m_values[9] = zaxis[1];
            m_values[10] = zaxis[2];
            m_values[11] = 0;
            
            m_values[12] = billboardPos[0];
            m_values[13] = billboardPos[1];
            m_values[14] = billboardPos[2];
            m_values[15] = 1;


            return *this;
        }

        mat4x4& axisAlignBillboard( vec3& cameraPos, vec3& billboardPos, vec3& axis )
        {

            //get camera kaxis
            //axis cross camerakaxis = iaxis
            //jaxis - axis
            //kaxis = iaxis cross jaxis
            vec3 normalizeCameraPos = cameraPos;

            normalizeCameraPos.normalize();
            axis.normalize();

            vec3 xaxis = axis.crossproduct( normalizeCameraPos );
            xaxis.normalize();

            vec3 yaxis = axis;

            vec3 zaxis = xaxis.crossproduct( yaxis );

            m_values[0] = xaxis[0];
            m_values[1] = xaxis[1];
            m_values[2] = xaxis[2];
            m_values[3] = 0;
            
            m_values[4] = yaxis[0];
            m_values[5] = yaxis[1];
            m_values[6] = yaxis[2];
            m_values[7] = 0;
            
            m_values[8] = zaxis[0];
            m_values[9] = zaxis[1];
            m_values[10] = zaxis[2];
            m_values[11] = 0;
            
            m_values[12] = billboardPos[0];
            m_values[13] = billboardPos[1];
            m_values[14] = billboardPos[2];
            m_values[15] = 1;

            return *this;
        }

        mat3x3 extractRotationMatrix()
        {
            float values[9] =
            {
                m_values[0], m_values[1], m_values[2],
                m_values[4], m_values[5], m_values[6],
                m_values[8], m_values[9], m_values[10]
            };

            return mat3x3( values );
        }

        vec3 extractTranslationVector()
        {
            return vec3( m_values[12], m_values[13], m_values[14] );
        }

        float* data()
        {
            return m_values;
        }

    private:

        static const int SIZE = 4;

        float m_values[SIZE * SIZE];

    };
}