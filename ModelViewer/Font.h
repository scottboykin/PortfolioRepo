#pragma once

#include <map>
#include <string>
#include <vector>
#include "vec2.h"
#include "vec3.h"

class TiXmlNode;
class TiXmlElement;

using namespace SBMath;

namespace SBGraphics
{
    struct Glyph
    {

    public:

        Glyph( 
            int initIndex = 0, 
            int initSheet = 0, 
            vec2 initMinTexCoords = vec2(), 
            vec2 initMaxTexCoords = vec2(), 
            float initTTFA = 0, 
            float initTTFB = 0,
            float initTTFC = 0
            );

        int index;
        int sheet;

        vec2 minTexCoords;
        vec2 maxTexCoords;

        float ttfA;
        float ttfB;
        float ttfC;

    };


    class Font
    {

    public:

        Font();
        Font( const char* fontBitmapFileName, const char* fontXMLDefFileName );

        void loadFont( const char* fontBitmapFileName, const char* fontXMLDefFileName );
        void drawString( const char* string, vec2 position, vec3 color, float size );
        void draw3dString( const char* string, vec3 position, vec3 up, vec3 normal, vec3 color, float size );
        void drawWorldAlignedString( const char* string, vec3 position, vec3 eyePos, vec3 color, float size );
        void drawAxisAlignedString( const char* string, vec3 position, vec3 eyePos, vec3 axis, vec3 color, float size );
        float measureString( const char* string, float size );

    private:

        void generateCharVertices( std::vector< float >& vertices, Glyph& glyph, vec3& position, vec3& direction, vec3& up, float& size );
        void loadFontInfo( TiXmlElement* element );
        void createGlyph( TiXmlElement* element );
        bool parseFontData( TiXmlNode* parent );
        bool loadFontBitmap( const char* fileName);

        unsigned int m_fontBitmapID;
        std::map< int, Glyph > m_glyphMap;

        std::string m_fontName;
        std::string m_locale;

        int m_numOfTexSheets;
        int m_cellPixelHeight;
    };
}