#version 430 core
layout(vertices = 16) out;
layout (std140, binding = 1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
} perFrame;

in vec2 mapCoord_TC[];
out vec2 mapCoord_TE[];

const int AB = 2;
const int BC = 3;
const int CD = 0;
const int DA = 1;

const int tessellationFactor = 2;
const float tessellationSlope = 1.0f;
const float tessellationShift = 0.1f;

float lodFactor(float dist)
{
    return max(0.0, tessellationFactor/pow(dist, tessellationSlope) + tessellationShift);
}

void main()
{
    if(gl_InvocationID == 0)
    {
        vec3 abMid = vec3(gl_in[0].gl_Position + gl_in[3].gl_Position)/2.0;
        vec3 bcMid = vec3(gl_in[3].gl_Position + gl_in[15].gl_Position)/2.0;
        vec3 cdMid = vec3(gl_in[15].gl_Position + gl_in[12].gl_Position)/2.0;
        vec3 daMid = vec3(gl_in[12].gl_Position + gl_in[0].gl_Position)/2.0;

        float distanceAB = distance(abMid, perFrame.cameraPosition);
        float distanceBC = distance(bcMid, perFrame.cameraPosition);
        float distanceCD = distance(cdMid, perFrame.cameraPosition);
        float distanceDA = distance(daMid, perFrame.cameraPosition);

        gl_TessLevelOuter[AB] = mix(1, gl_MaxTessGenLevel, lodFactor(distanceAB));
        gl_TessLevelOuter[BC] = mix(1, gl_MaxTessGenLevel, lodFactor(distanceBC));
        gl_TessLevelOuter[CD] = mix(1, gl_MaxTessGenLevel, lodFactor(distanceCD));
        gl_TessLevelOuter[DA] = mix(1, gl_MaxTessGenLevel, lodFactor(distanceDA));

        gl_TessLevelInner[0] = (gl_TessLevelOuter[BC] + gl_TessLevelOuter[DA])/4;
        gl_TessLevelInner[1] = (gl_TessLevelOuter[AB] + gl_TessLevelOuter[CD])/4;
    }
    mapCoord_TE[gl_InvocationID] = mapCoord_TC[gl_InvocationID];
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}