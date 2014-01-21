#version 330
#pragma optionnv unroll all 

const int MAX_LIGHTS = 5;
const int MAX_BONES = 4;
const int MAX_BONE_TRANSFORM = 100;

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
in ivec4 boneIndices;
in vec4 boneWeights;
uniform int numberOfLights = 2;
uniform vec3 camera_position;
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform mat4 mvpMatrix;
uniform Light lights[MAX_LIGHTS];
uniform mat4 bonetm[100];
uniform float time;
uniform bool isSkinned = false;
out vec2 textureCoord;
out vec3 normal_vector;
out vec3 vecToCameraInWorldSpace;
out vec3 vecToLightInWorldSpace[MAX_LIGHTS];
out vec3 lightDirectionInWorldSpace[MAX_LIGHTS];
out vec4 positionInScreenSpace;
out vec3 positionInWorldSpace;
out vec4 debugColor;

void main()
{
	vec4 newVertex = vec4( vertex, 1.0 );

	if( isSkinned )
	{
		vec3 position = vec3( 0, 0, 0 );
		
		for( int i = 0; i < 4; ++i )
		{
			int boneIndex = boneIndices[i];
			float boneWeight = boneWeights[i];

			position += ( bonetm[boneIndex] * vec4( vertex, 1 ) * boneWeight ).xyz;
		}
		newVertex = vec4( position, 1 );

		gl_Position = Projection * View * Model * newVertex;
		//if( boneWeights[2] > 0 )
		//{
		//	debugColor = vec4( 1, 0, 0, 1);
		//}
		//else
		//{
		//	debugColor = vec4( 0, 0, 0, 1);
		//}
	}

	else
	{
		gl_Position = Projection * View * Model * newVertex;
	}

    vec4 vertexInWorldSpace = Model * newVertex;
	vecToCameraInWorldSpace = camera_position - vertexInWorldSpace.xyz;
                                            
	for( int i = 0; i < MAX_LIGHTS; ++i )
	{
		if( i >= numberOfLights )
		{
			break;
		}

		if( lights[i].color.a == 0 )
		{
			vecToLightInWorldSpace[i]  = -lights[i].direction.xyz;
		}
		else if( lights[i].color.a == 1 )
		{
			vecToLightInWorldSpace[i]  = lights[i].position.xyz - vertexInWorldSpace.xyz;
		}
		else if( lights[i].color.a == 2 )
		{
			vecToLightInWorldSpace[i]  = lights[i].position.xyz - vertexInWorldSpace.xyz;
			lightDirectionInWorldSpace[i] = lights[i].direction.xyz;
		}		

		vecToLightInWorldSpace[i] = normalize( vecToLightInWorldSpace[i] );
		lightDirectionInWorldSpace[i] = normalize( lightDirectionInWorldSpace[i] );
	}

    normal_vector =  normalize( ( Model * vec4( normal, 0 ) ).xyz );
	positionInWorldSpace = vertexInWorldSpace.xyz;
	positionInScreenSpace = View * vertexInWorldSpace;
    textureCoord = texcoord.xy;
}