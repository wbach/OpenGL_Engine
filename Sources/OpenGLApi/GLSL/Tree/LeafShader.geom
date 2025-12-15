#version 440 core

const vec3 WORLD_UP = vec3(0, 1, 0);
const int SEGMENTS = 4;

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures;
    vec4 viewDistance;
    vec4 shadowVariables;
    vec4 fogData;
} perApp;

layout (std140, align=16, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
    vec4 projection;
} perFrame;

out GS_OUT
{
    vec2 texCoord;
    vec4 worldPos;
    vec3 normal;
    vec3 color;
} gs_out;

in VS_OUT
{
    vec3 worldPosition;
    vec2 sizeAndRotation; // x = size, y = rotation (radians)
    vec3 normal;
    vec3 color;
} gs_in[];

void EmitBillboardVertex(vec3 worldPos, vec2 uv)
{
    gl_Position = perFrame.projectionViewMatrix * vec4(worldPos, 1.0);

    gs_out.texCoord = uv;
    gs_out.worldPos = vec4(worldPos, 1.0);
    gs_out.normal   = normalize(perFrame.cameraPosition - worldPos);
    gs_out.color    = vec3(0, 1, 0);//gs_in[0].color;

    EmitVertex();
}

void main()
{
    vec3 center = gl_in[0].gl_Position.xyz;

    float size     = 1.f; //gs_in[0].sizeAndRotation.x;
    float rotation = 0.f;//gs_in[0].sizeAndRotation.y;

    // billboard basis
    vec3 toCamera = normalize(perFrame.cameraPosition - center);
    vec3 right    = normalize(cross(WORLD_UP, toCamera));
    vec3 up       = normalize(cross(toCamera, right));

    // rotation around view axis
    float c = cos(rotation);
    float s = sin(rotation);

    vec3 r = right * c + up * s;
    vec3 u = -right * s + up * c;

    r *= size;
    u *= size;

    // triangle strip order
    EmitBillboardVertex(center - r - u, vec2(0.0, 0.0));
    EmitBillboardVertex(center + r - u, vec2(1.0, 0.0));
    EmitBillboardVertex(center - r + u, vec2(0.0, 1.0));
    EmitBillboardVertex(center + r + u, vec2(1.0, 1.0));

    EndPrimitive();
}
