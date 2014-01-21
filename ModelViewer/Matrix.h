#pragma once

#include <cassert>
#include <math.h>
#include "Utilities.h"
#include "vec3.h"

namespace sb
{
    template < typename T, int rowNum, int colNum > class Matrix
    {

    public:

        explicit Matrix( T initialValue = 0.0f )
            :   m_numOfRows( rowNum )
            ,   m_numOfCols( colNum )
        {
            for( int i = 0; i < m_numOfRows * m_numOfCols; ++i )
            {
                *( m_values + i ) = initialValue;
            }
        }

        explicit Matrix( T* values )
            :   m_numOfRows( rowNum )
            ,   m_numOfCols( colNum )
        {
            for( int i = 0; i < m_numOfRows * m_numOfCols; ++i )
            {
                m_values[i] = values[i];
            }
        }

        Matrix ( Matrix& other )
            :   m_numOfRows( rowNum )
            ,   m_numOfCols( colNum )
        {
            for( int i = 0; i < m_numOfRows * m_numOfCols; ++i )
            {
                m_values[i] = other.m_values[i];
            }
        }

        ~Matrix()
        {}
        
        T& operator []( const int i ) { return m_values[i]; }


        Matrix& operator =( Matrix& other )
        {
            for( int i = 0; i < m_numOfRows * m_numOfCols; ++i )
            {
                m_values[i] = other.m_values[i];
            }

            return *this;
        }

        Matrix operator*( Matrix& other )
        {
            assert( m_numOfCols == other.m_numOfRows );

            T result[ rowNum * colNum ];

            for( int i = 0; i < rowNum * other.m_numOfCols; ++i )
            {
                result[i] = 0;
            }

            for( int i = 0; i < m_numOfRows; ++i )
            {
                for( int j = 0; j < other.m_numOfCols; ++j )
                {
                    for( int k = 0; k < m_numOfCols; ++k )
                    {
                        result[i * other.m_numOfCols + j] += m_values[i * m_numOfCols + k] * other.m_values[k * other.m_numOfCols + j];
                    }
                }
            }

            return Matrix( result );
        }

        Matrix operator*( T constant )
        {
            Matrix result;
            int size = m_numOfRows * m_numOfCols;
            for( int i = 0; i < size; ++i )
            {
                result[i] = *(m_values + i) * constant;
            }
            return result;
        }

        vec3 operator*( vec3& vector )
        {
            assert( m_numOfRows == 3 );
            assert( m_numOfCols == 3 );

            T result[ 3 ];

            result[0] = m_values[0] * vector[0] + m_values[1] * vector[1] + m_values[2] * vector[2]; 
            result[1] = m_values[3] * vector[0] + m_values[4] * vector[1] + m_values[5] * vector[2]; 
            result[2] = m_values[6] * vector[0] + m_values[7] * vector[1] + m_values[8] * vector[2]; 

            return vec3( result[0], result[1], result[2] );
        }

        Matrix operator+( Matrix& other )
        {
            assert( m_rows == other.m_rows );
            assert( m_cols == other.m_cols );

            Matrix result( m_rows, m_cols );

            int size = m_rows* m_cols;

            for( int i = 0; i < size; ++i )
            {
                result[i] = *(m_values + i) + *(other.m_values + i);
            }
            return result;
        }

        static Matrix< float, 4, 4 > worldAlignBillboard( vec3& cameraPos, vec3& billboardPos )
        {
            vec3 zaxis( cameraPos[0] - billboardPos[0], cameraPos[1] - billboardPos[1], cameraPos[2] - billboardPos[2] );
            zaxis.normalize();

            vec3 worldUp( 0.0f, 1.0f, 0.0f );
            vec3 xaxis = worldUp.crossproduct( zaxis );
            xaxis.normalize();

            vec3 yaxis = zaxis.crossproduct( xaxis );
            yaxis.normalize();

            float result[16];

            result[0] = xaxis[0];
            result[1] = xaxis[1];
            result[2] = xaxis[2];
            result[3] = 0;

            result[4] = yaxis[0];
            result[5] = yaxis[1];
            result[6] = yaxis[2];
            result[7] = 0;

            result[8] = zaxis[0];
            result[9] = zaxis[1];
            result[10] = zaxis[2];
            result[11] = 0;

            result[12] = billboardPos[0];
            result[13] = billboardPos[1];
            result[14] = billboardPos[2];
            result[15] = 1;

            return Matrix< float, 4, 4 >( result );
        }

        static Matrix< float, 4, 4 > axisAlignBillboard( vec3& cameraPos, vec3& billboardPos, vec3& axis )
        {

            //get camera kaxis
            //axis cross camerakaxis = iaxis
            //jaxis - axis
            //kaxis = iaxis cross jaxis

            cameraPos.normalize();
            axis.normalize();

            vec3 xaxis = axis.crossproduct( cameraPos );
            xaxis.normalize();

            vec3 yaxis = axis;

            vec3 zaxis = xaxis.crossproduct( yaxis );

            float result[16];
            result[0] = xaxis[0];
            result[1] = xaxis[1];
            result[2] = xaxis[2];
            result[3] = 0;

            result[4] = yaxis[0];
            result[5] = yaxis[1];
            result[6] = yaxis[2];
            result[7] = 0;

            result[8] = zaxis[0];
            result[9] = zaxis[1];
            result[10] = zaxis[2];
            result[11] = 0;

            result[12] = billboardPos[0];
            result[13] = billboardPos[1];
            result[14] = billboardPos[2];
            result[15] = 1;

            return Matrix< float, 4, 4 > ( result );
        }

        float* values() { return m_values; }
        int rows() { return m_numOfRows; }
        int cols() { return m_numOfCols; }

    private:

        T m_values[ rowNum * colNum ];
        int m_numOfRows;
        int m_numOfCols;

    };
}