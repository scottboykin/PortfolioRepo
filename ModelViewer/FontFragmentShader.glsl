#version 330
 
in vec2 textureCoord;
in vec4 color;
in vec3 worldPosition;
uniform sampler2D diffuseTex;
uniform sampler2D noiseTex;
uniform float uHighPoint = 0;
uniform float uLowPoint = 1;
uniform float time;
out vec4 fragColor;

void main () 
{
	vec4 textureColor = texture( diffuseTex, textureCoord );

	float alpha = textureColor.a;

	if( alpha == 0 )
		discard;
	
	const float timeScaler = 0.075;
	vec4 fireColor = vec4( 0.5, 0.1, 0.02, 1 );
	vec2 noiseCoord = vec2( textureCoord.x, cos( time * timeScaler + textureCoord.y ) );

	float spread = uHighPoint - uLowPoint;
	fireColor *= 18 * ( ( worldPosition.y - uHighPoint ) / spread ) * texture( noiseTex, noiseCoord ).r;

	fragColor = color + fireColor;
	fragColor.a = alpha;
}