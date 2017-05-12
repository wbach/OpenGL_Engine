#version 330 core
const int MAX_BONES = 100;

layout (location = 0) in vec3 Position;                                             
layout (location = 1) in vec2 TexCoord;   
layout (location = 2) in vec3 Normal;    

out vec2 TexCoord0;
out vec3 Normal0;    

uniform mat4 TransformationMatrix ;
uniform mat4 ProjectionMatrix ;
uniform mat4 ViewMatrix ;

void main(void)
{
	gl_Position =  ProjectionMatrix * ViewMatrix * TransformationMatrix * vec4(Position, 1.0);
	Normal0     = (TransformationMatrix * vec4(Normal, 0.0)).xyz;   
    TexCoord0   = TexCoord;
}