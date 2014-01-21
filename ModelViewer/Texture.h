#ifndef TEXTURE_H
#define TEXTURE_H

#include <glew.h>
#include <glut.h>

#include <string>
#include <map>

namespace SBGraphics
{
    class Texture
    {
    public:

        typedef unsigned int Size;		// Used for the width and height
        typedef unsigned int Color;	// RGBA color

        //
        // Member Functions ///////////////////////////////////////////////
        //

        Texture();
        explicit Texture( const std::string& texturePath );
        // REQUIRES( !texturePath.empty() );

        ~Texture();

        bool IsLoaded() const;
        // Returns true if the texture has been successfully loaded through 
        // one of the Load...() functions below 
        // and is ready to be applied for rendering.

        std::string GetFilePath() const;
        // Returns the path of the file (if any) from which this
        // texture was loaded.
        // Returns an empty string if no file was used in loading this
        // texture or if the texture has not been loaded.

        unsigned int getTextureID(){ return textureID; }
        Size GetWidth() const;
        Size GetHeight() const;
        // REQUIRES( IsLoaded() );
        // PROMISES( IsPowerOfTwo( result ));

        bool HasAlpha() const;
        // REQUIRES( IsLoaded() );
        // Returns true if the loaded texture should be 
        // rendered with alpha blending enabled (slower!).
        // This would be true if the original texture file
        // had an alpha channel (RGBA rather than RGB texture).

        void LoadFromMemory( Size width, Size height, Color* pRGBATexels );
        // REQUIRES( !IsLoaded() );
        // REQUIRES( width > 0 );
        // REQUIRES( height > 0 );
        // REQUIRES( IsPowerOfTwo( width ));
        // REQUIRES( IsPowerOfTwo( height ));
        // REQUIRES( pRGBATexels );
        // PROMISES( IsLoaded() );
        // The buffer pointed to by pRGBATexels 
        // *may be* modified by this function.

        void LoadFromFile( const std::string& texturePath );
        // REQUIRES( !IsLoaded() );
        // REQUIRES( !texturePath.empty() );
        // PROMISES( IsLoaded() );
        // PROMISES( GetFilePath() == texturePath );

        //
        // Static Member Functions ////////////////////////////////////////////
        //

        static void ApplyTexture( Texture* pTexture, GLenum target );
        // REQUIRES( pTexture == 0 || pTexture->IsLoaded() );
        // If pTexture is 0, texturing will be disabled.
        // Otherwise, texturing is enabled and the indicated
        // texture is applied.
        // If pTexture->HasAlpha(), alpha blending will also
        // be enabled. Otherwise alpha blending will be disabled.

        static Texture* CreateOrGetTexture( const std::string& texturePath );
        // REQUIRES( !texturePath.empty() );
        // REQUIRES( strlen( szTexturePath ) > 0 );
        // PROMISES( result );
        // PROMISES( result->IsLoaded() );
        // PROMISES( result->GetFilePath() == szTexturePath );
        // If the given path has already been loaded, this function
        // returns the previously loaded texture.
        // Otherwise, this function loads the file into a new texture,
        // records the association between the texture with the path, 
        // and returns the texture.

        static void DestroyTexture( Texture*& pTexture );
        // REQUIRES( pTexture );
        // PROMISES( pTexture == 0 );
        // Deletes the indicated texture.
        // If this texture was registered through the
        // CreateOrGetTexture() mechanism, DestroyTexture()
        // will remove the texture from this registration.
        // Textures created through CreateOrGetTexture()
        // should always be deleted using DestroyTexture().

        static void DestroyAllTextures();
        // Destroys all textures registered through the CreateOrGetTexture() 
        // function.

    private:

        // Do not implement! Prevents copying.
        Texture( const Texture& );
        void operator=( const Texture& );

        Size width;
        Size height;
        Color* textureColor;
        std::string textureFilePath;
        bool textureLoaded;
        bool renderWithAlpha;
        unsigned int textureID;
        static std::map < std::string, Texture* > textureMap;
};

}
#endif