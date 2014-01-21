#version 330
 
in vec2 textureCoord;
in vec4 color;
uniform sampler2D diffuseTex;
uniform float time;
uniform int useTexture = 0;
out vec4 fragColor;

void main () 
{	
    vec4 textureColor = texture( diffuseTex, textureCoord );
	
	if( useTexture == 1 )
	{
		fragColor = textureColor * color;
	}
	else
	{
		fragColor = color;
	}
}