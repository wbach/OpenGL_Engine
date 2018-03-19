#version 120
uniform mat4 transformationMatrix;
uniform vec2 translation;
uniform vec3 color;
varying vec4 colour;

void main()
{
	mat4 mvp = transformationMatrix * gl_ModelViewMatrix;
	vec4 position = vec4(gl_Vertex.xy, 0.0, 1.0);	
	vec2 posxy = (mvp * position).xy;
	
	gl_Position = vec4( posxy + translation, 0.0, 1.0);
	colour = vec4(color, 1.f);   
	gl_TexCoord[0] = gl_MultiTexCoord0;
}