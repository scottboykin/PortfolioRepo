#include <iostream>

#include "Font.h"
#include "tinyxml.h"
#include "mat4x4.h"
#include "Renderer.h"
#include "MaterialManager.h"
#include "Material.h"

#define STBI_HEADER_FILE_ONLY
#include "stb_image.c"

using namespace SBMath;
using namespace SBGraphics;

namespace SBGraphics
{
    Glyph::Glyph(
                  int initIndex, 
                  int initSheet, 
                  vec2 initMinTexCoords, 
                  vec2 initMaxTexCoords, 
                  float initTTFA, 
                  float initTTFB,
                  float initTTFC 
                 )
        :   index( initIndex )
        ,   sheet( initSheet )
        ,   minTexCoords( initMinTexCoords )
        ,   maxTexCoords( initMaxTexCoords )
        ,   ttfA( initTTFA )
        ,   ttfB( initTTFB )
        ,   ttfC( initTTFC )
    {}

    Font::Font()
        :   m_fontBitmapID( 0 )
        ,   m_glyphMap()
        ,   m_fontName()
        ,   m_locale()
        ,   m_numOfTexSheets( 0 )
        ,   m_cellPixelHeight( 0 )
    {}

    Font::Font( const char* fontBitmapFileName, const char* fontXMLDefFileName )
        :   m_fontBitmapID( 0 )
        ,   m_glyphMap()
        ,   m_fontName()
        ,   m_locale()
        ,   m_numOfTexSheets( 0 )
        ,   m_cellPixelHeight( 0 )
    {
        loadFont( fontBitmapFileName, fontXMLDefFileName );
    }

    void Font::loadFont( const char* fontBitmapFileName, const char* fontXMLDefFileName )
    {
        m_glyphMap.clear();

        if( !loadFontBitmap( fontBitmapFileName ) )
        {
            OutputDebugStringA( "Bitmap did not load! Font could not be loaded!" );
            std::cout << "Bitmap did not load! Font could not be loaded!" << std::endl;
            return;
        }

        TiXmlDocument fontDef( fontXMLDefFileName );

        if( fontDef.LoadFile() )
        {
            parseFontData( &fontDef );
        }
    }

    void Font::drawString( const char* string, vec2 position, vec3 color, float size )
    {
        Renderer& renderer = Renderer::getRenderer();

        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glDisable( GL_DEPTH_TEST );

        size_t length = strlen( string );
        unsigned int index = 0;
        vec3 currentDrawPosition = vec3( position[0], position[1], 0 );
        std::vector< GLfloat > vertices;
        GLsizei numVertices = 0;
        vec3 direction( 1.0, 0.0, 0.0 );
        vec3 up( 0.0, -1.0, 0.0 );

        while( index < length )
        {
            if( string[index] == '\n' )
            {
                currentDrawPosition[0] = position[0];
                currentDrawPosition[1] += size;
            }
            else
            {
                generateCharVertices( vertices, m_glyphMap[ string[index] ], currentDrawPosition, direction, up, size );
                numVertices += 6;
            }
            ++index;
        }

        std::vector< vec3 > colors;
        for( int i = 0; i < numVertices; ++i )
        {
            colors.push_back( color );
        }

        Material& material = renderer.getMaterialManager().getMaterial( "basic" );

        int useTex = 1;
        material.setUniform( std::string( "useTexture" ), Material::Uniformi, &useTex );
        material.setTexture( std::string( "diffuseTex" ), GL_TEXTURE0, m_fontBitmapID );
        material.setUniform( std::string( "Projection" ), Material::UniformMatrix4f, renderer.projectionMatrixStack.getTop().data() );
        material.setUniform( std::string( "View" ), Material::UniformMatrix4f, renderer.viewMatrixStack.getTop().data() );
        material.setUniform( std::string( "Model" ), Material::UniformMatrix4f, renderer.modelMatrixStack.getTop().data() );

        material.setVertexAttribPointer( std::string( "vertex" ), 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 5, vertices.data() );
        material.setVertexAttribPointer( std::string( "texcoord" ), 2, GL_FLOAT, GL_FALSE, sizeof( float ) * 5, vertices.data() + 3 );
        material.setVertexAttribPointer( std::string( "inColor" ), 3, GL_FLOAT, GL_FALSE, 0, colors.data() );

        renderer.drawVertexArray( material, GL_TRIANGLES, 0, numVertices );
        
    }

    void Font::draw3dString( const char* string, vec3 position, vec3 up, vec3 normal, vec3 color, float size )
    {
        Renderer& renderer = Renderer::getRenderer();

        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glEnable( GL_DEPTH_TEST );

        vec3 direction = up.crossproduct( normal );

        size_t length = strlen( string );
        unsigned int index = 0;
        vec3 currentDrawPosition = vec3( position[0], position[1], position[2] );
        std::vector< GLfloat > vertices;
        GLsizei numVertices = 0;

        while( index < length )
        {
            if( string[index] == '\n' )
            {
                currentDrawPosition -= up * size;
                currentDrawPosition[0] = position[0];
            }
            else
            {
                generateCharVertices( vertices, m_glyphMap[ string[index] ], currentDrawPosition, direction, up, size );
                numVertices += 6;
            }
            ++index;
        }

        std::vector< vec3 > colors;
        for( int i = 0; i < numVertices; ++i )
        {
            colors.push_back( color );
        }

        Material& material = renderer.getMaterialManager().getMaterial( "fiery" );

        int useTex = 1;
        material.setUniform( std::string( "useTexture" ), Material::Uniformi, &useTex );
        material.setTexture( std::string( "diffuseTex" ), GL_TEXTURE0, m_fontBitmapID );
        material.setUniform( std::string( "Projection" ), Material::UniformMatrix4f, renderer.projectionMatrixStack.getTop().data() );
        material.setUniform( std::string( "View" ), Material::UniformMatrix4f, renderer.viewMatrixStack.getTop().data() );
        material.setUniform( std::string( "Model" ), Material::UniformMatrix4f, renderer.modelMatrixStack.getTop().data() );

        material.setVertexAttribPointer( std::string( "vertex" ), 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 5, vertices.data() );
        material.setVertexAttribPointer( std::string( "texcoord" ), 2, GL_FLOAT, GL_FALSE, sizeof( float ) * 5, vertices.data() + 3 );
        material.setVertexAttribPointer( std::string( "inColor" ), 3, GL_FLOAT, GL_FALSE, 0, colors.data() );

        renderer.drawVertexArray( material, GL_TRIANGLES, 0, numVertices );
    }

    void Font::drawWorldAlignedString( const char* string, vec3 position, vec3 eyePos, vec3 color, float size )
    {
        //Implement a measure string method to pass the middle of the string as the position
        mat4x4 transformMatrix;
        transformMatrix.worldAlignBillboard( eyePos, vec3( position[0] + measureString( string, size ) / 2, position[1] - size / 2, position[2] ) );

        Renderer::getRenderer().modelMatrixStack.pushMatrix();
        Renderer::getRenderer().modelMatrixStack.loadMatrix( mat4x4( transformMatrix.data() ) );

        draw3dString( string, vec3(), vec3( 0, 1, 0 ), vec3( 0, 0, 1 ), color, size );

        Renderer::getRenderer().modelMatrixStack.popMatrix();
    }

    void Font::drawAxisAlignedString( const char* string, vec3 position, vec3 eyePos, vec3 axis, vec3 color, float size )
    {
        //Implement a measure string method to pass the middle of the string as the position
        mat4x4 transformMatrix;
        transformMatrix.axisAlignBillboard( eyePos, position, axis );
        
        Renderer::getRenderer().modelMatrixStack.pushMatrix();
        Renderer::getRenderer().modelMatrixStack.loadMatrix( mat4x4( transformMatrix.data() ) );

        draw3dString( string, vec3(), vec3( 0, 1, 0 ), vec3( 0, 0, 1 ), color, size );

        Renderer::getRenderer().modelMatrixStack.popMatrix();
    }

    void Font::generateCharVertices( std::vector< GLfloat >& vertices, Glyph& glyph, vec3& position, vec3& direction, vec3& up, float& size )
    {
        position += direction * glyph.ttfA * size;
        vec3 topLeftVertex = position;
        vec3 topRightVertex = position + direction * glyph.ttfB * size;
        vec3 bottomLeftVertex = position - up * size;
        vec3 bottomRightVertex = position + direction * glyph.ttfB * size - up * size;

        vertices.push_back( topLeftVertex[0] );
        vertices.push_back( topLeftVertex[1] );
        vertices.push_back( topLeftVertex[2] );
        vertices.push_back( glyph.minTexCoords[0] );
        vertices.push_back( glyph.minTexCoords[1] );
        
        vertices.push_back( topRightVertex[0] );
        vertices.push_back( topRightVertex[1] );
        vertices.push_back( topRightVertex[2] );
        vertices.push_back( glyph.maxTexCoords[0] );
        vertices.push_back( glyph.minTexCoords[1] );
        
        vertices.push_back( bottomLeftVertex[0] );
        vertices.push_back( bottomLeftVertex[1] );
        vertices.push_back( bottomLeftVertex[2] );
        vertices.push_back( glyph.minTexCoords[0] );
        vertices.push_back( glyph.maxTexCoords[1] );
        
        vertices.push_back( bottomLeftVertex[0] );
        vertices.push_back( bottomLeftVertex[1] );
        vertices.push_back( bottomLeftVertex[2] );
        vertices.push_back( glyph.minTexCoords[0] );
        vertices.push_back( glyph.maxTexCoords[1] );

        vertices.push_back( topRightVertex[0] );
        vertices.push_back( topRightVertex[1] );
        vertices.push_back( topRightVertex[2] );
        vertices.push_back( glyph.maxTexCoords[0] );
        vertices.push_back( glyph.minTexCoords[1] );

        vertices.push_back( bottomRightVertex[0] );
        vertices.push_back( bottomRightVertex[1] );
        vertices.push_back( bottomRightVertex[2] );
        vertices.push_back( glyph.maxTexCoords[0] );
        vertices.push_back( glyph.maxTexCoords[1] );

        position += direction * glyph.ttfB * size + direction * glyph.ttfC * size;
    }

    float Font::measureString( const char* string, float size )
    {
        size_t length = strlen( string );
        unsigned int index = 0;
        vec3 currentPosition = vec3( 0, 0, 0 );

        while( index < length )
        {
            Glyph currentChar = m_glyphMap[ string[index] ];
            currentPosition[0] += ( currentChar.ttfA + currentChar.ttfB + currentChar.ttfC ) * size;

            ++index;
        }

        return currentPosition[0];
    }

    bool Font::loadFontBitmap( const char* fileName )
    {
        unsigned char* texels;
        int width, height, channel;
        texels = stbi_load( fileName, &width, &height, &channel, 0 );

        if( texels )
        {
            glGenTextures( 1, &m_fontBitmapID );
            glBindTexture( GL_TEXTURE_2D, m_fontBitmapID );
            glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            stbi_image_free( texels );
            return true;
        }

        return false;
    }

    void Font::loadFontInfo( TiXmlElement* element )
    {
        if ( !element ) return;

        TiXmlAttribute* pAttrib = element->FirstAttribute();
        while (pAttrib)
        {
            if( !strcmp( pAttrib->Name(), "name" ) )
            {
                m_fontName = pAttrib->Value();
            }

            else if( !strcmp( pAttrib->Name(), "locale" ) )
            {
                m_locale = pAttrib->Value();
            }

            else if( !strcmp( pAttrib->Name(), "numTextureSheets" ) )
            {
                m_numOfTexSheets = atoi( pAttrib->Value() );
            }

            else if( !strcmp( pAttrib->Name(), "cellPixelHeight" ) )
            {
                m_cellPixelHeight = atoi( pAttrib->Value() );
            }

            pAttrib=pAttrib->Next();
        }
    }

    void Font::createGlyph( TiXmlElement* element )
    {
        if ( !element ) return;

        Glyph newGlyph;

        TiXmlAttribute* pAttrib = element->FirstAttribute();
        
        while (pAttrib)
        {
            if( !strcmp( pAttrib->Name(), "ucsIndex" ) )
            {
                newGlyph.index = atoi( pAttrib->Value() );
            }

            else if( !strcmp( pAttrib->Name(), "sheet" ) )
            {
                newGlyph.sheet = atoi( pAttrib->Value() );
            }

            else if( !strcmp( pAttrib->Name(), "texCoordMins" ) )
            {
                const char* vec2String = pAttrib->Value();
                int stringIndex = 0;
                for( int i = 0; i < 2; ++i )
                {
                    std::string xyString;
                    while( ( vec2String[stringIndex] >= '0' && vec2String[stringIndex] <= '9' ) || vec2String[stringIndex] == '.' )
                    {
                        xyString += vec2String[stringIndex];
                        ++stringIndex;
                    }

                    if( i == 0 )
                    {
                        while( vec2String[stringIndex] < '0' || vec2String[stringIndex] > '9' )
                        {
                            ++stringIndex;
                        }
                    }
                    newGlyph.minTexCoords[i] = (float)atof( xyString.c_str() );
                }
            }

            else if( !strcmp( pAttrib->Name(), "texCoordMaxs" ) )
            {
                const char* vec2String = pAttrib->Value();
                int stringIndex = 0;
                for( int i = 0; i < 2; ++i )
                {
                    std::string xyString;
                    while( ( vec2String[stringIndex] >= '0' && vec2String[stringIndex] <= '9' ) || vec2String[stringIndex] == '.' )
                    {
                        xyString += vec2String[stringIndex];
                        ++stringIndex;
                    }

                    if( i == 0 )
                    {
                        while( vec2String[stringIndex] < '0' || vec2String[stringIndex] > '9' )
                        {
                            ++stringIndex;
                        }
                    }
                    newGlyph.maxTexCoords[i] = (float)atof( xyString.c_str() );
                }
            }

            else if( !strcmp( pAttrib->Name(), "ttfA" ) )
            {
                newGlyph.ttfA = (float)atof( pAttrib->Value() );
            }

            else if( !strcmp( pAttrib->Name(), "ttfB" ) )
            {
                newGlyph.ttfB = (float)atof( pAttrib->Value() );
            }

            else if( !strcmp( pAttrib->Name(), "ttfC" ) )
            {
                newGlyph.ttfC = (float)atof( pAttrib->Value() );
            }

            pAttrib=pAttrib->Next();
        }	
        m_glyphMap.insert( std::pair< int, Glyph >( newGlyph.index, newGlyph ) );
    }

    bool Font::parseFontData( TiXmlNode* parent )
    {
        TiXmlNode* pChild;
        int t = parent->Type();

        switch ( t )
        {
        case TiXmlNode::TINYXML_ELEMENT:
            if( !strcmp( parent->Value(), "FontInfo" ) )
            {
                loadFontInfo( parent->ToElement() );
            }
            else if( !strcmp( parent->Value(), "Glyph" ) )
            {
                createGlyph( parent->ToElement() );
            }
        }
        
        for ( pChild = parent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
        {
            parseFontData( pChild );
        }
        
        return true;
    }
}