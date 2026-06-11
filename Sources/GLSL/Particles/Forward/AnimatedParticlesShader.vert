#version 330 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in mat4 ModelViewMatrix;
layout (location = 6) in vec4 TextureOffset;
layout (location = 7) in float BlendFactor;

uniform mat4 ProjectionMatrix;
uniform float NumberOfRows;

out vec4 textureCoords;
out float blendFactor;

void main(void)
{
    vec2 scaledTextureCoords = TexCoord / NumberOfRows;
    textureCoords.xy = scaledTextureCoords + TextureOffset.xy;
    textureCoords.zw = scaledTextureCoords + TextureOffset.zw;

    blendFactor = BlendFactor;
    gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(Position, 1.0);
}