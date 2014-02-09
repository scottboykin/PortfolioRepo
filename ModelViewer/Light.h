#pragma once

#include "vec3.h"
#include "vec4.h"

namespace SBGraphics
{
    class Light 
    {

    public:

        float attenuation;
        float nearFalloff;
        float farFalloff;
        float aperture;

        Light();

        enum LightTypes
        {
            Directional = 0,
            Point,
            Spotlight
        };

        void setPosition( SBMath::vec3& position ){ m_position = position; }
        SBMath::vec3& getPosition(){ return m_position; }

        void setDirection( SBMath::vec3& direction ){ m_direction = direction; }
        SBMath::vec3& getDirection(){ return m_direction; }

        void setColor( SBMath::vec3& color )
        {
            m_color.x = color.x;
            m_color.y = color.y;
            m_color.z = color.z;
        }
        SBMath::vec4& getColor(){ return m_color; }

        void setType( LightTypes type ){ m_color.w = (float)type; }

    private:

        SBMath::vec3 m_position;
        SBMath::vec3 m_direction;
        SBMath::vec4 m_color;

    };
}