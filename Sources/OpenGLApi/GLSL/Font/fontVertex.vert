#version 120
uniform mat4 transformationMatrix;
uniform vec2 translation;
uniform float textSize;

void main()
{
    mat4 mvp = transformationMatrix * gl_ModelViewMatrix;
    vec4 position = vec4(gl_Vertex.xy, 0.0, 1.0);
    vec2 posxy = (mvp * position).xy + translation * 1.0 / textSize;
    posxy = posxy * textSize;
    gl_Position = vec4(posxy, 0.0, 1.0);
    gl_TexCoord[0] = gl_MultiTexCoord0;
}