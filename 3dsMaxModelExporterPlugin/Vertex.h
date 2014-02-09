#pragma once

struct Vertex
{
    Vertex()
        :   x( 0 )
        ,   y( 0 )
        ,   z( 0 )
        ,   nx( 0 )
        ,   ny( 0 )
        ,   nz( 0 )
        ,   tu( 0 )
        ,   tv( 0 )
        ,   r( 1 )
        ,   g( 1 )
        ,   b( 1 )
        ,   a( 1 )
        ,   matID( 0 )
    {}

    float x;
    float y;
    float z;
    float nx;
    float ny;
    float nz;
    float tu;
    float tv;
    float r;
    float g;
    float b;
    float a;

    int matID;

};