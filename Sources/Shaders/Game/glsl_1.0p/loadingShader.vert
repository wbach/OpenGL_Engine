#version 130

in vec3 position;
in vec2 textureCoords;

uniform mat4 transformationMatrix;

out vec2 TexCoordOut;

void main()
{
    gl_Position = transformationMatrix * vec4(position, 1.0);
    TexCoordOut = textureCoords;
}