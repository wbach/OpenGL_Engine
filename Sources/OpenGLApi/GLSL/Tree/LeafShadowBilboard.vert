#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal; 

layout (std140, align=16, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
    vec4 projection;
    vec4 time;
} perFrame;

layout (std140, binding = 3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

layout(std140, align=16, binding = 4) uniform LeafParams
{
    vec4 wind;
    vec4 fparams;
    ivec4 atlasParams;
    float time;
} leafParams;

out vec2 TexCoords;
flat out int TexIndex;

void main()
{
    TexCoords = texCoords;
    TexIndex = int(normal.x); 
    
    vec4 worldPos = perObjectUpdate.transformationMatrix * vec4(position, 1.0);

    float phase = dot(worldPos.xz, vec2(0.12, 0.15));
    float wave = sin(leafParams.time * 2.0 + phase);
    vec3 windOffset = leafParams.wind.xyz * leafParams.wind.w * wave;
    
    worldPos.xyz += windOffset;

    gl_Position = perFrame.projectionViewMatrix * worldPos;
}