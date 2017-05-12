#version 120
uniform mat4 transformationMatrix ;
uniform vec2 translation ;
varying vec4 colour ;
void main()
{
// gl_Position = vec4(position + translation * vec2(2.0 , -2.0),0.0,1.0) ;
  //  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(gl_Vertex.xy,1,1);
	
	gl_Position = vec4((transformationMatrix*gl_ModelViewMatrix * vec4(gl_Vertex.xy, 0.0,1.0)).xy  + translation ,0.0,1.0)  ;
	colour = gl_Color;   
	gl_TexCoord[0] = gl_MultiTexCoord0;
}