#version 330                                                                        
                                                                                    
layout (location = 0) in vec3 Position;                                             
layout (location = 1) in vec2 TexCoord;                                             
layout (location = 2) in vec3 Normal;                                               
layout (location = 3) in vec3 Tangent;

uniform mat4 TransformationMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

out vec2 TexCoord0;                                                                 
out vec3 Normal0;                                                                   
out vec4 WorldPos0;

//Fog
out float Distance;

void main()
{    
	vec4 world_pos = TransformationMatrix* vec4(Position, 1.0);
	vec4 model_view_position  = ViewMatrix * world_pos;
    gl_Position    = ProjectionMatrix * model_view_position;
    TexCoord0      = TexCoord;                  
    Normal0        = (TransformationMatrix * vec4(Normal, 0.0)).xyz;   
    WorldPos0      = world_pos;

	Distance = length(model_view_position.xyz) ;
}