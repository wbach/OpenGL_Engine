#version 330

layout (location = 0) in vec3 Position;                                             
layout (location = 1) in vec2 TexCoord;                                             
layout (location = 2) in vec3 Normal;                                               
layout (location = 3) in vec3 Tangent;
layout (location = 4) in mat4 TransformationMatrixes;

uniform mat4 TransformationMatrix ;
uniform mat4 ProjectionMatrix ;
uniform mat4 ViewMatrix ;
uniform vec3 cameraPosition ;

out vec2 TexCoord0;                                                                 
out vec3 Normal0;                                                                   
out vec3 WorldPos0;   
out vec4 ClipSpace;
out vec3 toCameraVector;

const float tiling = 10.0;
const float viewDistance = 450.0 ;
const float transitionDistance = 10.0;
const float density = 0.0035 ;
const float gradient = 2.5 ;

void main(void)
 {	
    TexCoord0		= TexCoord * tiling;//TexCoord;                  
    Normal0			= (TransformationMatrix * vec4(Normal, 0.0)).xyz;   
    WorldPos0		= (TransformationMatrix * vec4(Position, 1.0)).xyz;  
	toCameraVector	= cameraPosition - WorldPos0; 
	ClipSpace		= ProjectionMatrix * ViewMatrix * vec4(WorldPos0, 1.0f) ;
	gl_Position		= ClipSpace;
}