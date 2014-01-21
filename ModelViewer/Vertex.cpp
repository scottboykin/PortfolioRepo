#include "Vertex.h"

using namespace SBMath;

namespace SBGraphics
{
    Vertex::Vertex()
        :   materialID( 0 )
    {}

    Vertex::Vertex( vec3& initPosition, vec2& initTexCoords, vec4& initColor )
        :   position( initPosition )
        ,   texcoords( initTexCoords )
        ,   color( initColor )
        ,   materialID( 0 )
    {}
}