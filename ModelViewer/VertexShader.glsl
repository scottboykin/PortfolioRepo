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

in vec3 vertex;
in vec3 normal;
in vec3 texcoord;
in vec3 _tangent;
in vec3 _bitangent;
in vec3 _normal;
uniform int numberOfLights = 2;
uniform vec3 camera_position;
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform mat4 mvpMatrix;
uniform Light lights[MAX_LIGHTS];
uniform float time;
out vec2 textureCoord;
out vec3 normal_vector;
out vec3 vecToCameraInTangentSpace;
out vec3 vecToLightInTangentSpace[MAX_LIGHTS];
out vec3 lightDirectionInTangentSpace[MAX_LIGHTS];
out vec4 positionInScreenSpace;
out vec3 positionInWorldSpace;
 
 
void main() 
{
    gl_Position = Projection * View * Model * vec4( vertex, 1.0 );
 
    mat3 tbnInverse = inverse( mat3( _tangent, _bitangent, _normal ) );
 
    vec4 vertexInWorldSpace = Model * vec4( vertex, 1.0 );
	vecToCameraInTangentSpace = tbnInverse * ( inverse( Model ) * vec4( camera_position - vertexInWorldSpace.xyz, 0 ) ).xyz;
                                            
	for( int i = 0; i < MAX_LIGHTS; ++i )
	{
		if( i >= numberOfLights )
		{
			break;
		}

		if( lights[i].color.a == 0 )
		{
			vecToLightInTangentSpace[i]  = tbnInverse * ( inverse( Model ) * vec4( -lights[i].direction.xyz, 0 ) ).xyz;
		}
		else if( lights[i].color.a == 1 )
		{
			vecToLightInTangentSpace[i]  = tbnInverse * ( inverse( Model ) * vec4( lights[i].position.xyz - vertexInWorldSpace.xyz, 0 ) ).xyz;
		}
		else if( lights[i].color.a == 2 )
		{
			vecToLightInTangentSpace[i]  = tbnInverse * ( inverse( Model ) * vec4( lights[i].position.xyz - vertexInWorldSpace.xyz, 0 ) ).xyz;
			lightDirectionInTangentSpace[i] = tbnInverse * ( inverse( Model ) * vec4( lights[i].direction.xyz, 0 ) ).xyz;
		}		

		vecToLightInTangentSpace[i] = normalize( vecToLightInTangentSpace[i] );
		lightDirectionInTangentSpace[i] = normalize( lightDirectionInTangentSpace[i] );
	}

    normal_vector = inverse( transpose( tbnInverse ) ) * normal;
	positionInWorldSpace = vertexInWorldSpace.xyz;
	positionInScreenSpace = View * vertexInWorldSpace;
    textureCoord = texcoord.xy;
}