#version 330
 
in vec3 vertex;
in vec4 inColor;
in vec2 texcoord;
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform float time;
out vec2 textureCoord;
out vec4 color;
 
void main() 
{
    gl_Position = Projection * View * Model * vec4( vertex, 1.0 );
    textureCoord = texcoord;
	color = inColor;
}