#version 330 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform vec4 TextureOffset;
uniform float NumberOfRows;

out vec4 position;
out vec4 textureCoords;

void main(void)
{
    position =  ProjectionMatrix * ModelViewMatrix * vec4(Position, 1.0);
    vec2 scaledTextureCoords = TexCoord / NumberOfRows;
    textureCoords.xy = scaledTextureCoords + TextureOffset.xy;
    textureCoords.zw = scaledTextureCoords + TextureOffset.zw;

	gl_Position = position;
}