#version 330
 
in vec2 textureCoord;
in vec4 color;
uniform sampler2D diffuseTex;
uniform sampler2D depthTex;
uniform float time;
uniform int useTexture = 0;
uniform int blur = 1;
out vec4 fragColor;

void main () 
{	
	vec2 texCoord = textureCoord;
    vec4 textureColor = texture( diffuseTex, texCoord );
	
	if( useTexture == 1 )
	{
		if( blur == 1 )
		{
			int blurAmount = 20;
			for( int i = blurAmount / -2; i < blurAmount / 2; ++i )
			{
				vec2 blurTexCoord = texCoord;
				blurTexCoord.x += i * 1.0 / 1024;
				textureColor += texture( diffuseTex, blurTexCoord );
			}

			textureColor /= blurAmount;
		}
		fragColor = textureColor * color;
	}
	else
	{
		fragColor = color;
	}
}