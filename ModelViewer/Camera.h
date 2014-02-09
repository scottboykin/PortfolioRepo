#include "vec2.h"
#include "vec3.h"
#include "mat3x3.h"
#include "mat4x4.h"

namespace SBGraphics
{    
    class Camera
    {

    public:

        bool mouseLeftBDown;
        bool mouseRightBDown;
        bool bLockMouse;
        unsigned char keys[256];

        Camera( SBMath::vec3& position, SBMath::vec3& rotation, SBMath::vec2& mousePosition );

        void updateInput();  
        void updateMousePos( SBMath::vec2& mousePos );
        bool isKeyDown( char key );
        void setScreenCenter( int x, int y );

        SBMath::vec3& rotation() { return m_rotation; }
        SBMath::vec3& position() { return m_position; }
        SBMath::mat4x4 getView();
        SBMath::vec3 getNextPos( float delta );

        static Camera& getStaticCamera();


    private:

        float m_drag;

        SBMath::vec2 m_screenCenter;

        SBMath::vec2 m_mousePos;
        SBMath::vec2 m_prevMousePos;

        SBMath::vec3 m_rotation;
        SBMath::vec3 m_position;
        SBMath::vec3 m_velocity;
        SBMath::vec3 m_lookingAt;
    };
}