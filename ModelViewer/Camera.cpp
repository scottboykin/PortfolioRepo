#include <glew.h>
#include <glut.h>

#include "Camera.h"

using namespace SBMath;

namespace SBGraphics
{
    Camera::Camera( vec3& position = vec3(), vec3& rotation = vec3(), vec2& mousePosition = vec2( 100.0f ) )
        :   m_mousePos( mousePosition )
        ,   m_position( position )
        ,   m_rotation( rotation )
        ,   bLockMouse( true )
        ,   m_drag( 0.5f )
    {}

    bool Camera::isKeyDown( char virtualKey )
    {
        return keys[virtualKey] == 1;
    }

    void Camera::updateInput()
    {
        if( bLockMouse )
        {
            const float MOUSE_YAW_POWER = 0.2f;
            const float MOUSE_PITCH_POWER = 0.2f;
            //const float MOVE_SPEED = 0.05f;
            const float MOVE_SPEED = 2.0f;

            vec2 mouseDelta = m_mousePos - m_prevMousePos;

            // Update camera yaw.
            //
            m_rotation.y -= mouseDelta.y * MOUSE_YAW_POWER;
            m_rotation.x -= mouseDelta.x * MOUSE_PITCH_POWER;

            m_rotation.y = clamp( m_rotation.y, -89.5f, 89.5f );

            vec3 displacement;

            if( isKeyDown( 'w' ) || mouseLeftBDown )
            {
                displacement.z -= 1.0f;
            }

            if( isKeyDown( 's' ) || mouseRightBDown )
            {
                displacement.z += 1.0f;
            }

            if( isKeyDown( 'a' ) )
            {
                displacement.x -= 1.0f;
            }

            if( isKeyDown( 'd' ) )
            {
                displacement.x += 1.0f;
            }

            if( isKeyDown( 'q' ) )
            {
                displacement.y -= 1.0f;
            }

            if( isKeyDown( 'e' ) )
            {
                displacement.y += 1.0f;
            }

            displacement.normalize();
            displacement = displacement * MOVE_SPEED;

            mat3x3 rotationmatrix;
            rotationmatrix.fromEulerAngles( degreeToRadians( m_rotation.x ), degreeToRadians( m_rotation.y ), degreeToRadians( m_rotation.z ) );

            vec3 finalimpulse = rotationmatrix * displacement;

            m_velocity += finalimpulse;
            m_velocity -= m_velocity * m_drag;
            m_position += m_velocity;

        
            m_mousePos = m_screenCenter;
            glutWarpPointer( (int)m_screenCenter.x, (int)m_screenCenter.y );
        }
    }

    void Camera::updateMousePos( SBMath::vec2& mousePos )
    {
        m_prevMousePos = m_mousePos;
        m_mousePos = mousePos;
    }

    void Camera::setScreenCenter( int x, int y )
    {
        m_screenCenter.x = (float)x;
        m_screenCenter.y = (float)y;
    }

    mat4x4 Camera::getView()
    {       
        vec3 target = getNextPos( 1.0f );
        vec3 up( 0.0, 1.0, 0.0 );

        vec3 zaxis = (m_position - target).normalize();
        vec3 xaxis = up.crossproduct( zaxis ).normalize();
        vec3 yaxis = zaxis.crossproduct( xaxis ).normalize();

        if( xaxis.x == 0 && xaxis.y == 0 && xaxis.z == 0 )
        {
            xaxis = up.crossproduct( zaxis - vec3( 0.0f, 0.01f, 0.0f ) );
        }

        float rValues[] =
        {
            xaxis.x, yaxis.x, zaxis.x, 0,
            xaxis.y, yaxis.y, zaxis.y, 0,
            xaxis.z, yaxis.z, zaxis.z, 0,
            0,       0,       0, 1
        };

        float tValues[] =
        {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            -m_position.x, -m_position.y, -m_position.z, 1
        };

        return mat4x4( tValues ) * mat4x4( rValues);
    }

    vec3 Camera::getNextPos( float delta )
    {
        mat3x3 rotationmatrix;
        rotationmatrix.fromEulerAngles( degreeToRadians( m_rotation.x ), degreeToRadians( m_rotation.y ), degreeToRadians( m_rotation.z ) );

        vec3 impulse = rotationmatrix * vec3( 0, 0, 1 ) * delta;
        vec3 nextPos( 
            m_position[0] - impulse[0], 
            m_position[1] - impulse[1],
            m_position[2] - impulse[2] );

        return nextPos;
    }

    Camera& Camera::getStaticCamera()
    {
        static Camera camera;
        return camera;
    }
}