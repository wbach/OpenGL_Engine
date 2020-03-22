#version 440
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in mat4 ModelViewMatrix;
layout (location = 6) in vec4 TextureOffset;
layout (location = 7) in float BlendFactor;

layout (std140, align=16, binding=6) uniform InputBuffer
{
    mat4 ProjectionMatrix;
    float NumberOfRows;
} inputBuffer;

out vec4 position;
out vec4 textureCoords;
out float blendFactor;

void main(void)
{
    position =  inputBuffer.ProjectionMatrix * ModelViewMatrix * vec4(Position, 1.0);
    vec2 scaledTextureCoords = TexCoord / inputBuffer.NumberOfRows;
    textureCoords.xy = scaledTextureCoords + TextureOffset.xy;
    textureCoords.zw = scaledTextureCoords + TextureOffset.zw;

    blendFactor = BlendFactor;
    gl_Position = position;
}