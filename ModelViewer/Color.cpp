#include "Color.h"

namespace SBGraphics
{
    Color::Color( char red, char green, char blue )
        :   r( red )
        ,   g( green )
        ,   b( blue )
        ,   a( 255 )
    {}

    Color::Color( char red, char green, char blue, char alpha )
        :   r( red )
        ,   g( green )
        ,   b( blue )
        ,   a( alpha )
    {}

    Color::Color( Colors color )
        :   r( 0 )
        ,   g( 0 )
        ,   b( 0 )
        ,   a( 255 )
    {
        switch( color )
        {
            case WHITE:
                r = 255;
                g = 255;
                b = 255;
                break;

            case RED:
                r = 255;
                g = 0;
                b = 0;
                break;

            case GREEN:
                r = 0;
                g = 255;
                b = 0;
                break;

            case BLUE:
                r = 0;
                g = 0;
                b = 255;
                break;
        }
    }

    SBMath::vec4i Color::getByteColor()
    {
        return SBMath::vec4i( r, g, b, a );
    }

    SBMath::vec4 Color::getFloatColor()
    {
        return SBMath::vec4( r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f );
    }
}