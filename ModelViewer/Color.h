#pragma once

#include "vec4.h"
#include "vec4i.h"

namespace SBGraphics
{
    class Color
    {

    public:

        enum Colors 
        {
            BLACK,
            WHITE,
            RED,
            GREEN,
            BLUE            
        };

        Color( char red = 0, char green = 0, char blue = 0 );
        Color( char red, char green, char blue, char alpha );
        Color( Colors color );

        SBMath::vec4i getByteColor();
        SBMath::vec4 getFloatColor();

    private:

        unsigned char r, g, b, a;

    };
}