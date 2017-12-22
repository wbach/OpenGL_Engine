#version 410 core                                                                     
                                                                                    
layout (location = 0) in vec3 Position;                                             
layout (location = 1) in vec2 TexCoord;                                             
layout (location = 2) in vec3 Normal;                                             

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 TransformationMatrix;

uniform vec4 viewport;

out Vertex
{
	vec3 position;
	vec2 textCoord;
	vec3 normal;
	vec2 winPosition;
	mat4 viewProjection;
} Out;

vec2 Project(const mat4 modelViewProjectionMatrix)
{
    // transformacja współrzędnych
    vec4 win = modelViewProjectionMatrix * vec4(Position, 1.f);
    if( win[3] == 0.0 ) return vec2( 0.0 );
    win /= win[3];

    // mapowanie x, y, z do przedziału <0;1>
    win.xyz *= 0.5;
    win.xyz += 0.5;

    // mapowanie x,y do obszaru renderingu
    win[0] = win[0] * viewport[2] + viewport[0];
    win[1] = win[1] * viewport[3] + viewport[1];
    return win.xy;
}

void main()
{     
	Out.viewProjection 				= ProjectionMatrix * ViewMatrix;
	mat4 modelViewProjectionMatrix 	= Out.viewProjection * TransformationMatrix;
	
	Out.position 	= (TransformationMatrix * vec4(Position, 1.f)).xyz;
	Out.normal 		= (TransformationMatrix * vec4(Normal, 1.f)).xyz;
	Out.winPosition = Project(modelViewProjectionMatrix);
	Out.textCoord 	= TexCoord;

	gl_Position 	= vec4(Out.position, 1.f);
}