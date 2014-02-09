#include "Texture.h"
#include <cassert>
#include <iostream>
#include "Utilities.h"

#define STBI_HEADER_FILE_ONLY
#include "stb_image.c"

#define REQUIRES assert
#define PROMISES assert

    //
    // Member Functions ///////////////////////////////////////////////
    //
namespace SBGraphics
{
	
	Texture::Texture() : width(0), height(0), textureLoaded(false), renderWithAlpha(false), textureFilePath(""), textureColor(0), textureID(0)
	{ }

	// REQUIRES( !texturePath.empty() );
	Texture::Texture( const std::string& texturePath ) : width(0), height(0), textureLoaded(false), renderWithAlpha(false), textureFilePath(""), textureColor(0), textureID(0)
	{
		REQUIRES( !texturePath.empty() );
		LoadFromFile(texturePath);
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &textureID);
	}

	// Returns true if the texture has been successfully loaded through 
	// one of the Load...() functions below 
	// and is ready to be applied for rendering.
	bool Texture::IsLoaded() const { return textureLoaded; }
		
	// Returns the path of the file (if any) from which this
	// texture was loaded.
	// Returns an empty string if no file was used in loading this
	// texture or if the texture has not been loaded.
	std::string Texture::GetFilePath() const { return textureFilePath; }

	// REQUIRES( IsLoaded() );
	// PROMISES( IsPowerOfTwo( result ));
	Texture::Size Texture::GetWidth() const 
	{
		REQUIRES( IsLoaded() );
		return width; 
	}

	// REQUIRES( IsLoaded() );
	// PROMISES( IsPowerOfTwo( result ));
	Texture::Size Texture::GetHeight() const 
	{ 
		REQUIRES( IsLoaded() );
		return height; 
	}

	// REQUIRES( IsLoaded() );
	// Returns true if the loaded texture should be 
	// rendered with alpha blending enabled (slower!).
	// This would be true if the original texture file
	// had an alpha channel (RGBA rather than RGB texture).
	bool Texture::HasAlpha() const 
	{ 
		REQUIRES( IsLoaded() );
		return renderWithAlpha; 
	}

	// REQUIRES( !IsLoaded() );
	// REQUIRES( width > 0 );
	// REQUIRES( height > 0 );
	// REQUIRES( IsPowerOfTwo( width ));
	// REQUIRES( IsPowerOfTwo( height ));
	// REQUIRES( pRGBATexels );
	// PROMISES( IsLoaded() );
	// The buffer pointed to by pRGBATexels 
	// *may be* modified by this function.
	void Texture::LoadFromMemory( Size width, Size height, Color* pRGBATexels )
	{
		REQUIRES( !IsLoaded() );
		REQUIRES( width > 0 );
		REQUIRES( height > 0 );
		REQUIRES( pRGBATexels );

		GLuint idTexture;

		glGenTextures( 1, &idTexture );
		assert( idTexture );

		// Configure the texture object.
		//
		glBindTexture(GL_TEXTURE_2D, idTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Load the texture object from memory.
		//
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pRGBATexels );

		//Check for alpha
		for ( Size i = 0; i < height; ++i )
		{
			for ( Size j = 0; j < width; ++j )
			{
				if( ( pRGBATexels[ j + i * width ] & 0xFF000000 ) != 0xFF000000 )
				{
					renderWithAlpha = true;
					break;
				}
			}
			if ( renderWithAlpha )
				break;
		}

		this->width = width;
		this->height = height;
		textureID = idTexture;
		textureLoaded = true;

		PROMISES( IsLoaded() );
	}
		
	// REQUIRES( !IsLoaded() );
	// REQUIRES( !texturePath.empty() );
	// PROMISES( IsLoaded() );
	// PROMISES( GetFilePath() == texturePath );
	void Texture::LoadFromFile( const std::string& texturePath )
	{
		REQUIRES( !IsLoaded() );
		REQUIRES( !texturePath.empty() );

		int width, height, nChannels;
		unsigned char *texels = stbi_load ( texturePath.c_str(), &width, &height, &nChannels, 4 );

		GLuint idTexture = 0;

		if ( texels )
		{
			//Send to video card
			//
			glGenTextures( 1, &idTexture );
			assert( idTexture );

			// Configure the texture object.
			//
			glBindTexture(GL_TEXTURE_2D, idTexture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			// Load the texture object from memory.
			//
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels );

			if( nChannels == 4 )
				renderWithAlpha = true;

			stbi_image_free( texels );

			this->width = width;
			this->height = height;
			textureID = idTexture;
			textureLoaded = true;
			textureFilePath = texturePath;

			PROMISES( IsLoaded() );
		}

		else
		{
			trace( texturePath << " file not found!" );			
		}
	}

	//
	// Static Member Functions ////////////////////////////////////////////
	//

	// REQUIRES( pTexture == 0 || pTexture->IsLoaded() );
	// If pTexture is 0, texturing will be disabled.
	// Otherwise, texturing is enabled and the indicated
	// texture is applied.
	// If pTexture->HasAlpha(), alpha blending will also
	// be enabled. Otherwise alpha blending will be disabled.
	void Texture::ApplyTexture( Texture* pTexture, GLenum target )
	{
		REQUIRES( pTexture == 0 || pTexture->IsLoaded() );
		if( pTexture == 0 )
			glDisable( GL_TEXTURE_2D );
			
		else
		{
			glEnable( GL_TEXTURE_2D );
            glActiveTexture( target );
			glBindTexture( GL_TEXTURE_2D, pTexture->textureID );
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			if( pTexture->HasAlpha() )
			{
				glEnable( GL_BLEND );
				glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			}

			else
				glDisable( GL_BLEND );
		}
	}


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
	Texture* Texture::CreateOrGetTexture( const std::string& texturePath )
	{
		REQUIRES( !texturePath.empty() );

		auto mapElement = textureMap.find( texturePath );

		if( mapElement == textureMap.end() )
		{
			Texture* result = new Texture( texturePath );
			mapElement = textureMap.insert( std::pair < std::string , Texture* > ( texturePath, result ) ).first;
		}
			

		PROMISES( mapElement->second );
		PROMISES( mapElement->second->IsLoaded() );
		PROMISES( mapElement->second->GetFilePath() == texturePath );

		return mapElement->second;
	}


	// REQUIRES( pTexture );
	// PROMISES( pTexture == 0 );
	// Deletes the indicated texture.
	// If this texture was registered through the
	// CreateOrGetTexture() mechanism, DestroyTexture()
	// will remove the texture from this registration.
	// Textures created through CreateOrGetTexture()
	// should always be deleted using DestroyTexture().
	void Texture::DestroyTexture( Texture*& pTexture )
	{
		REQUIRES( pTexture );

		std::string temp = pTexture->GetFilePath();

		auto textureMapElement = textureMap.find( pTexture->GetFilePath() );

		if( textureMapElement != textureMap.end() )
		{            
			delete textureMapElement->second;
				
			pTexture = nullptr;

			PROMISES( pTexture == 0 );
		}  
	}


	// Destroys all textures registered through the CreateOrGetTexture() 
	// function.
	void Texture::DestroyAllTextures()
	{
		for ( auto mapIter = textureMap.begin(); mapIter != textureMap.end(); ++mapIter)
			DestroyTexture( mapIter->second );
		textureMap.clear();
	}

	std::map < std::string, Texture* > Texture::textureMap;
	
}