#version 440 core
const float size = 0.35f;
const vec3 up = vec3(0, 1, 0);
const float pi = 3.14159265359;
const float v1angle = 3*pi/4;
const float v2angle = pi;
const float v3angle = pi / 4;
const float v4angle = 2*pi - pi /4;

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
    // if( offset.y > 0.f)
    // {
    //     float globalTime = grassShaderBuffer.variables.y;
    //     offset.x += sin(globalTime);
    //     offset.z += cos(globalTime);
    // }

    vec4 actual_offset = vec4(offset * size, .0f);
   // actual_offset.y += size;
    vec4 worldPosition = (gl_in[0].gl_Position + actual_offset);

    gl_Position = perFrame.projectionViewMatrix * worldPosition;

    gs_out.texCoord = vec2((textCoord.x + 1.0) / 2.0, 1 - (-textCoord.y + 1.0) / 2.0);
    gs_out.worldPos = worldPosition.xyz;
   // gs_out.worldPos.y += size;

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

vec4 CreateQuaternion(vec3 normal)
{
    vec4 q;
    vec3 a = cross(normal, up );
    q.x = a.x;
    q.y = a.y;
    q.z = a.z;
    q.w = sqrt((length(normal) * length(normal)) * (length(up) * length(up))) + dot(normal, up);
    return normalize(q);
}

vec4 multQuat(vec4 q1, vec4 q2)
{
    return vec4(
                q1.w * q2.x + q1.x * q2.w + q1.z * q2.y - q1.y * q2.z,
                q1.w * q2.y + q1.y * q2.w + q1.x * q2.z - q1.z * q2.x,
                q1.w * q2.z + q1.z * q2.w + q1.y * q2.x - q1.x * q2.y,
                q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z
               );
}

vec3 rotate_vector( vec4 quat, vec3 vec )
{
    vec4 qv = multQuat( quat, vec4(vec, 0.0) );
    return multQuat( qv, vec4(-quat.x, -quat.y, -quat.z, quat.w) ).xyz;
}

vec3 qtransform( vec4 q, vec3 v )
{
    return v + 2.0*cross(cross(v, q.xyz ) + q.w*v, q.xyz);
}

void CreateXYquad(vec4 rotation, vec2 textCoord[4])
{
    vec3 v11 = vec3(-0.5f, 1.f, 0.f);
    vec3 v22 = vec3(-0.5f, 0.f, 0.f);
    vec3 v33 = vec3(0.5f, 1.f, 0.f);
    vec3 v44 = vec3(0.5f, 0.f, 0.f);

    CreateVertex(rotate_vector(rotation, v11), textCoord[0]);
    CreateVertex(rotate_vector(rotation, v22), textCoord[1]);
    CreateVertex(rotate_vector(rotation, v33), textCoord[2]);
    CreateVertex(rotate_vector(rotation, v44), textCoord[3]);
    EndPrimitive();
}

void CreateZYquad(vec4 rotation, vec2 textCoord[4])
{
    vec3 v11 = vec3(0, 1.f, -0.5f);
    vec3 v22 = vec3(0, 0.f, -0.5f);
    vec3 v33 = vec3(0, 1.f, 0.5f);
    vec3 v44 = vec3(0, 0.f, 0.5f);

    CreateVertex(rotate_vector(rotation, v11), textCoord[0]);
    CreateVertex(rotate_vector(rotation, v22), textCoord[1]);
    CreateVertex(rotate_vector(rotation, v33), textCoord[2]);
    CreateVertex(rotate_vector(rotation, v44), textCoord[3]);
    EndPrimitive();
}

void CreateXYZquad(vec4 rotation, vec2 textCoord[4])
{
    vec3 v11 = vec3(-0.4f, 1.f, -0.4f);
    vec3 v22 = vec3(-0.4f, 0.f, -0.4f);
    vec3 v33 = vec3(0.4f, 1.f, 0.4f);
    vec3 v44 = vec3(0.4f, 0.f, 0.4f);

    CreateVertex(rotate_vector(rotation, v11), textCoord[0]);
    CreateVertex(rotate_vector(rotation, v22), textCoord[1]);
    CreateVertex(rotate_vector(rotation, v33), textCoord[2]);
    CreateVertex(rotate_vector(rotation, v44), textCoord[3]);
    EndPrimitive();
}

void CreateZYXquad(vec4 rotation, vec2 textCoord[4])
{
    vec3 v11 = vec3(0.4f, 1.f, -0.4f);
    vec3 v22 = vec3(0.4f, 0.f, -0.4f);
    vec3 v33 = vec3(-0.4f, 1.f, 0.4f);
    vec3 v44 = vec3(-0.4f, 0.f, 0.4f);

    CreateVertex(rotate_vector(rotation, v11), textCoord[0]);
    CreateVertex(rotate_vector(rotation, v22), textCoord[1]);
    CreateVertex(rotate_vector(rotation, v33), textCoord[2]);
    CreateVertex(rotate_vector(rotation, v44), textCoord[3]);
    EndPrimitive();
}

void main(void)
{
    gs_out.useShadows = perApp.shadowVariables.x;

    vec2 quadTextCoord[4];
    quadTextCoord[0] = vec2(-1,1);
    quadTextCoord[1] = vec2(-1,-1);
    quadTextCoord[2] = vec2(1,1);
    quadTextCoord[3] = vec2(1,-1);

   // vec3 normal = vec3(1, 1, 1);
    vec3 normal = vec3(0, 1, 0);
    vec4 quaternion = CreateQuaternion(normalize(normal));
    CreateXYquad(quaternion, quadTextCoord);
    CreateZYquad(quaternion, quadTextCoord);
    CreateXYZquad(quaternion, quadTextCoord);
    CreateZYXquad(quaternion, quadTextCoord);
}