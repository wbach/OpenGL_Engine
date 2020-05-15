#version 440 core
const float size = 0.35f;

layout (points) in;
layout (triangle_strip, max_vertices = 16) out;

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
    vec4 clipPlane;
} perApp;

layout (std140, align=16, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    mat4 toShadowMapSpace;
    vec3 cameraPosition;
} perFrame;

layout (std140, align=16, binding=6) uniform GrassShaderBuffer
{
    vec2 variables; // x = viewDistance, y - globalTime
} grassShaderBuffer;

out GS_OUT
{
    vec4 shadowCoords;
    float useShadows;
    float shadowMapSize;
    vec2 texCoord;
    vec3 worldPos;
} gs_out;

int CreateVertex(vec3 offset, vec2 textCoord)
{
    if( offset.y > 0.f)
    {
        float globalTime = grassShaderBuffer.variables.y;
        offset.x += sin(globalTime);
        offset.z += cos(globalTime);
    }

    vec4 actual_offset = vec4(offset * size, .0f);
    actual_offset.y += size;
    vec4 worldPosition = (gl_in[0].gl_Position + actual_offset);

    gl_Position = perFrame.projectionViewMatrix * worldPosition;

    gs_out.texCoord = vec2((textCoord.x + 1.0) / 2.0, 1 - (-textCoord.y + 1.0) / 2.0);
    gs_out.worldPos = worldPosition.xyz;
    gs_out.worldPos.y += size;

    if (perApp.shadowVariables.x > 0.5f)
    {
        gs_out.shadowMapSize = perApp.shadowVariables.z;

        float shadow_distance           = perApp.shadowVariables.y;
        const float transition_distance = 10.f;

        float distance_to_cam   = length(perFrame.cameraPosition - worldPosition.xyz);
        gs_out.shadowCoords     = perFrame.toShadowMapSpace * worldPosition;
        distance_to_cam         = distance_to_cam - (shadow_distance - transition_distance);
        distance_to_cam         = distance_to_cam / shadow_distance;
        gs_out.shadowCoords.w   = clamp(1.f - distance_to_cam, 0.f, 1.f);
    }
    EmitVertex();
    return 0;
}

void main(void)
{
    gs_out.useShadows = perApp.shadowVariables.x;

    vec2 quad_text_coord[4];
    quad_text_coord[0] = vec2(-1,1);
    quad_text_coord[1] = vec2(-1,-1);
    quad_text_coord[2] = vec2(1,1);
    quad_text_coord[3] = vec2(1,-1);

    //x - axis quad
    int pass = 0;
    pass += CreateVertex(vec3(-1.f, 1.f, 0.f), quad_text_coord[0] );
    if( pass < 0) return;
    CreateVertex(vec3(-1.f, -1.f, 0.f), quad_text_coord[1] );
    CreateVertex(vec3(1.f, 1.f, 0.f), quad_text_coord[2] );
    CreateVertex(vec3(1.f, -1.f, 0.f), quad_text_coord[3] );
    EndPrimitive();

    //z - axis quad
    CreateVertex(vec3(0.f, 1.f, -1.f), quad_text_coord[0] );
    CreateVertex(vec3(0.f, -1.f, -1.f), quad_text_coord[1] );
    CreateVertex(vec3(0.f, 1.f, 1.f), quad_text_coord[2] );
    CreateVertex(vec3(0.f, -1.f, 1.f), quad_text_coord[3] );
    EndPrimitive();

    //xz - axis quad
    CreateVertex(vec3(-1.f, 1.f, -1.f), quad_text_coord[0] );
    CreateVertex(vec3(-1.f, -1.f, -1.f), quad_text_coord[1] );
    CreateVertex(vec3(1.f, 1.f, 1.f), quad_text_coord[2] );
    CreateVertex(vec3(1.f, -1.f, 1.f), quad_text_coord[3] );
    EndPrimitive();

    //xz - axis quad
    CreateVertex(vec3(1.f, 1.f, -1.f), quad_text_coord[0] );
    CreateVertex(vec3(1.f, -1.f, -1.f), quad_text_coord[1] );
    CreateVertex(vec3(-1.f, 1.f, 1.f), quad_text_coord[2] );
    CreateVertex(vec3(-1.f, -1.f, 1.f), quad_text_coord[3] );
    EndPrimitive();
}