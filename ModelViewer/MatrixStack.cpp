#include "MatrixStack.h"

using namespace SBMath;

namespace sb
{
    MatrixStack::MatrixStack()
    {
        m_stack.push_back( mat4x4() );
    }

    void MatrixStack::pushMatrix()
    {
        m_stack.push_back( m_stack.back() );
    }

    void MatrixStack::popMatrix()
    {
        m_stack.pop_back();
    }

    void MatrixStack::translate( float x, float y, float z )
    {
        m_stack.back().translate( x, y, z );
    }

    void MatrixStack::rotate( float yaw, float pitch, float roll )
    {
        m_stack.back().rotate( yaw, pitch, roll );
    }

    void MatrixStack::scale( float xScale, float yScale, float zScale )
    {
        m_stack.back().scale( xScale, yScale, zScale );
    }

    void MatrixStack::loadIdentity()
    {
        m_stack.back().loadIdentity();
    }

    void MatrixStack::loadMatrix( SBMath::mat4x4& matrix )
    {
        m_stack.back() = matrix;
    }

    void MatrixStack::preMultipy( SBMath::mat4x4& matrix )
    {
        m_stack.back() = matrix * m_stack.back();
    }

    void MatrixStack::postMultipy( SBMath::mat4x4& matrix )
    {
        m_stack.back() = m_stack.back() * matrix;
    }
}