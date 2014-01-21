#version 330
#pragma optionnv unroll all

const int MAX_LIGHTS = 5;
 
struct Light
{
	vec3 position;
	vec3 direction;
	vec4 color;
	float attenuation;
	float aperture;
	float nearFalloff;
	float farFalloff;
};

in vec2 textureCoord;
in vec3 normal_vector;
in vec3 positionInWorldSpace;
in vec3 vecToCameraInWorldSpace;
in vec3 lightDirectionInWorldSpace[MAX_LIGHTS];
in vec3 vecToLightInWorldSpace[MAX_LIGHTS];
in vec4 positionInScreenSpace;
in vec4 debugColor;
uniform sampler2D diffuseTex;
uniform sampler2D normalTex;
uniform sampler2D emissiveTex;
uniform sampler2D specularTex;
uniform sampler2D heightTex;
uniform vec4 fogColor = vec4( 0.2f, 0.2f, 0.35f, 1.0 );
uniform Light lights[MAX_LIGHTS];
uniform int debug = 0;
uniform int numberOfLights = 2;
uniform float time;
uniform vec3 camera_position;
out vec4 fragColor;

const int useParallax = 0x00000001;
const int useDiffuse =  0x00000010;
const int useSpecular = 0x00000100;
const int useEmissive = 0x00001000;
const int useLighting = 0x00010000;
const int useFog =      0x00100000;

const float DIRECTIONAL = 0;
const float POINT = 0.1;
const float SPOTLIGHT = 0.2;

float scale = 0.04;
float bias = 0.02;
float individualLightContrib = 0.5;

void main () 
{	
	vec3 cameraNormalized = normalize( vecToCameraInWorldSpace );
	vec3 normal = normalize( normal_vector );

    vec4 diffuseColor = texture( diffuseTex, textureCoord );

	if( ( debug & useDiffuse ) != 0 )
	{
		diffuseColor = vec4( 1, 1, 1, 1 );
	}

	float totalDiffuseIntensity = 0;
	float totalSpecularIntensity = 0;
	vec4 totalLightColor = vec4( 0, 0, 0, 0 );
	//vec3 normalFromTexture = normalize( ( texture( normalTex, newTexCoord ).xyz ) * 2.0 - 1.0 );

	for( int i = 0; i < MAX_LIGHTS; ++i )
	{
		if( i >= numberOfLights )
		{
			break;
		}

		vec3 vecToLightNormalized = normalize( vecToLightInWorldSpace[i] );

		if( dot( vecToLightNormalized, normal ) <= 0 )
		{
			//continue;
		}

		if( ( debug & useLighting ) == 0 )
		{
			float diffuseIntensity = 0;

			if( lights[i].color.a == 0 )
			{
				diffuseIntensity = clamp( dot( normal, vecToLightNormalized ), 0, 1 );
			}

			if( lights[i].color.a == 1 )
			{
				diffuseIntensity = clamp( dot( normal, vecToLightNormalized ), 0, 1 );
				float dist = length( vecToLightInWorldSpace[i].xyz );
				//diffuseIntensity = dist;
			}
			
			if( lights[i].color.a == 2 )
			{
/*				vec3 vecToLightInWorldSpace = normalize( lights[i].position - positionInWorldSpace );
				vec3 lightForwardInWorldSpace = normalize( lights[i].direction );
				float apertureDot = dot( lightForwardInWorldSpace, -vecToLightInWorldSpace );
				if( apertureDot >= lights[i].aperture )
				{
					diffuseIntensity = clamp( dot( normalFromTexture, vecToLightNormalized ), 0, 1 );;
					diffuseIntensity *= lights[i].attenuation / distance( positionInScreenSpace.xyz, vecToLightInTangentSpace[i].xyz );
				}

*/
				float spotlightAngle = dot( -vecToLightNormalized, normalize( lightDirectionInWorldSpace[i] ) );
				if( spotlightAngle > lights[i].aperture )
				{
					diffuseIntensity = clamp( dot( normal, vecToLightNormalized ), 0, 1 );;
					diffuseIntensity *= lights[i].attenuation / distance( positionInScreenSpace.xyz, vecToLightInWorldSpace[i].xyz );
				}
//*/
			}
			
			totalDiffuseIntensity += diffuseIntensity * individualLightContrib;
			totalDiffuseIntensity = clamp( totalDiffuseIntensity, 0, 1 );
			totalLightColor += lights[i].color * diffuseIntensity;
			
			totalLightColor.r = clamp( totalLightColor.r, 0, 1 );
			totalLightColor.g = clamp( totalLightColor.g, 0, 1 );
			totalLightColor.b = clamp( totalLightColor.b, 0, 1 );
			totalLightColor.a = 1;
		}
		else
		{
		   totalDiffuseIntensity = 1;
		} 

		if( vecToLightNormalized.z > 0.0 && ( debug & useSpecular ) == 0 )
		{
			float specularIntensity = dot( normalize( cameraNormalized ), reflect( normal, vecToLightNormalized ) );
			specularIntensity = pow( specularIntensity, 32 );
			totalSpecularIntensity = clamp( totalSpecularIntensity + specularIntensity, 0, 1 );
		}
	}

	float fogContribution = clamp( positionInScreenSpace.z / -50, 0, 1 );
	if( ( debug & useFog ) != 0 )
	{
		fogContribution = 0;
	}
	
	float emissiveIntensity = 1;
	if( ( debug & useEmissive ) != 0 )
	{
		emissiveIntensity = 0;
	}

    fragColor = totalLightColor * diffuseColor * max( totalDiffuseIntensity, 0 );
	//fragColor = debugColor;
	//fragColor = ( 1 - fogContribution ) * fragColor + fogContribution * fogColor;
    fragColor.a = 1;
}