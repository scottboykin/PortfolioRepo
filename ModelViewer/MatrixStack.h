#pragma once

#include <vector>
#include "mat4x4.h"

namespace sb
{
    class MatrixStack
    {

    public:

        MatrixStack();

        void pushMatrix();
        void popMatrix();
        void translate( float x, float y, float z );
        void rotate( float yaw, float pitch, float roll );
        void scale( float xScale, float yScale, float zScale ); 
        void loadIdentity();
        void loadMatrix( SBMath::mat4x4& matrix );
        void preMultipy( SBMath::mat4x4& matrix );
        void postMultipy( SBMath::mat4x4& matrix );

        SBMath::mat4x4& getTop(){ return m_stack.back(); } 

    private:

        std::vector< SBMath::mat4x4 > m_stack;

    };
}