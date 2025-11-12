#version 440 core
const vec3 up = vec3(0, 1, 0);

layout (points) in;
layout (triangle_strip, max_vertices = 16) out;

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
    vec4 fogData; // xyz - color, w - gradient
} perApp;

layout (std140, align=16, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
} perFrame;

layout (std140, align=16, binding=6) uniform GrassShaderBuffer
{
    vec2 variables; // x = viewDistance, y - globalTime
} grassShaderBuffer;

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
    vec2 sizeAndRotation;
    vec3 normal;
    vec3 color;
} gs_in[];

int CreateVertex(vec3 offset, vec2 textCoord)
{
    if( offset.y > 0.5f)
    {
        offset.y = gs_in[0].sizeAndRotation.x;
    }
if (offset.y > 0.0f)
{
    float globalTime = grassShaderBuffer.variables.y;

    // --- Parametry wiatru ---
    vec3 windDir = normalize(vec3(0.7, 0.0, 0.3)); // kierunek
    float windStrength = 1.3;                      // globalna siła

    // --- Faza zależna od pozycji (dla zróżnicowania) ---
    float phase = dot(gs_in[0].worldPosition.xz, vec2(0.21, 0.17));

    // --- "Turbulencja" – kilka częstotliwości ---
    float windWaves =
        sin(globalTime * 1.3 + phase) * 0.6 +
        sin(globalTime * 0.7 + phase * 2.2) * 0.3 +
        sin(globalTime * 3.1 + phase * 0.9) * 0.1;

    // --- Gradient wysokości: dół sztywny, góra elastyczna ---
    float heightFactor = pow(clamp(offset.y / gs_in[0].sizeAndRotation.x, 0.0, 1.0), 2.5);

    // --- Ogranicz odchylenie, żeby nie wyglądało "płasko" ---
    float bendAmount = windStrength * 0.3; // mniejsze = mniej "prostokątne" bujanie

    // --- Efekt wiatr + szarpnięcie (lekki chaos) ---
    vec3 windOffset = windDir * windWaves * heightFactor * bendAmount;

    // --- Opcjonalny lekki obrót (pseudo-torsja) ---
    windOffset.xz += 0.05 * vec2(
        sin(globalTime * 2.1 + phase * 0.8),
        cos(globalTime * 2.4 + phase * 0.6)
    ) * heightFactor;

    offset += windOffset;
}

   // const float size = 0.35f;
    vec4 actual_offset = vec4(offset , .0f);
    offset.y -= 0.1f;
    vec4 worldPosition = (gl_in[0].gl_Position + actual_offset);

    gl_Position = perFrame.projectionViewMatrix * worldPosition;

    gs_out.texCoord = vec2((textCoord.x + 1.0) / 2.0, 1 - (-textCoord.y + 1.0) / 2.0);
    gs_out.worldPos = worldPosition;
    gs_out.normal   = gs_in[0].normal;
    gs_out.color    = gs_in[0].color;

    EmitVertex();
    return 0;
}

vec4 CreateQuaternion(vec3 normal)
{
    if(dot(normal, up) > 0.999999f && dot(normal, up) < -0.999999f)
    {
        return vec4(0, 0, 0, 1);
    }

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
    if (length(gs_in[0].worldPosition - perFrame.cameraPosition) < grassShaderBuffer.variables.x)
    {
        vec2 quadTextCoord[4];
        quadTextCoord[0] = vec2(-1,1);
        quadTextCoord[1] = vec2(-1,-1);
        quadTextCoord[2] = vec2(1,1);
        quadTextCoord[3] = vec2(1,-1);

       // vec3 normal = vec3(1, 1, 1);
        //vec3 normal = vec3(0, 1, 0);
        vec4 quaternion = CreateQuaternion(normalize(gs_in[0].normal));
        CreateXYquad(quaternion, quadTextCoord);
        CreateZYquad(quaternion, quadTextCoord);
        if (length(gs_in[0].worldPosition - perFrame.cameraPosition) < (grassShaderBuffer.variables.x) / 3.f)
        {
            CreateXYZquad(quaternion, quadTextCoord);
            CreateZYXquad(quaternion, quadTextCoord);
        }
    }
}