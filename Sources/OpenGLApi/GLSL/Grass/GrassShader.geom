#version 440 core
const vec3 up = vec3(0, 1, 0);
const int SEGMENTS = 4; // rtx 4070 tis max_vertices = 64 max

layout (points) in;
layout (triangle_strip, max_vertices = SEGMENTS * 16) out;

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
    vec4 projection;
} perFrame;

layout (std140, align=16, binding=6) uniform GrassShaderBuffer
{
    vec4 windParams;
    vec4 variables; // x = viewDistance, y - globalTime, z,w -dir
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
    vec4 actual_offset = vec4(offset, .0f);
    offset.y -= 0.05f;
    vec4 worldPosition = (gl_in[0].gl_Position + actual_offset);

    gl_Position = perFrame.projectionViewMatrix * worldPosition;

    gs_out.texCoord = textCoord;//vec2((textCoord.x + 1.0) / 2.0, 1 - (-textCoord.y + 1.0) / 2.0);
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

void CreateXYquad3(vec4 rotation, vec2 textCoord[4])
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

vec3 ApplyWind(vec3 offset, float heightFactor)
{
    float t = 6.f *  grassShaderBuffer.variables.y;

    // domyślne parametry
    float windStrength = grassShaderBuffer.windParams.x;
    float bendAmount = grassShaderBuffer.windParams.y;
    float turbulence = grassShaderBuffer.windParams.z;
    float torsion = grassShaderBuffer.windParams.w;

    // --- kierunek wiatru i faza zależna od pozycji w świecie ---
    vec3 windDir = normalize(vec3(grassShaderBuffer.variables.z, 0.f, grassShaderBuffer.variables.w));//normalize(vec3(0.7, 0.0, 0.3));
    float phase = dot(gs_in[0].worldPosition.xz, vec2(0.21, 0.17));

    // --- złożone falowanie w kilku częstotliwościach ---
    float windWaves =
        sin(t * 1.3 + phase) * 0.6 +
        sin(t * 0.7 + phase * 2.2) * 0.3 +
        sin(t * 3.1 + phase * 0.9) * 0.1;

    // --- dodatkowy podmuch przy wichurze ---
    if (windStrength > 0.5f)
        windWaves += sin(t * 6.0 + phase * 3.7) * 0.25 * turbulence;

    // --- gradient wysokości: dół sztywny, góra elastyczna ---
    float hFactor = pow(clamp(heightFactor, 0.0, 1.0), 2.5);

    // --- obliczenie przesunięcia ---
    vec3 windOffset = windDir * windWaves * bendAmount * hFactor * windStrength;

    // --- lekka torsja końcówek ---
    windOffset.xz += torsion * vec2(
        sin(t * 2.1 + phase * 0.8),
        cos(t * 2.4 + phase * 0.6)
    ) * hFactor;

    offset += windOffset;
    return offset;
}

int GetSegmentsByDistance()
{
    float dist = length(gs_in[0].worldPosition - perFrame.cameraPosition);

    // Zakładamy: maksymalna liczba segmentów w bliskiej odległości
    int maxSegments = SEGMENTS;
    int minSegments = 1; // minimum segmentów przy dużym dystansie

    // Możemy skalować liniowo: im dalej, tym mniej segmentów
    float maxDistance = grassShaderBuffer.variables.x; // np. viewDistance dla trawy
    float t = clamp(dist / maxDistance, 0.0, 1.0);

    // interpolacja segmentów: blisko = maxSegments, daleko = minSegments
    int segs = int(round(mix(float(maxSegments), float(minSegments), t)));

    return max(segs, 1); // nigdy nie mniej niż 1
}


void CreateSegmentedQuad(vec4 rotation, vec3 corner0, vec3 corner1, vec3 corner2, vec3 corner3)
{
    int segments = GetSegmentsByDistance();

    for (int i = 0; i < segments; i++)
    {
        float y0 = float(i) / float(segments);       // dolny segment
        float y1 = float(i + 1) / float(segments);   // górny segment

        // interpolacja lokalnych cornerów wzdłuż wysokości
        vec3 v11 = mix(corner0, corner1, y1); // górny lewy
        vec3 v22 = mix(corner0, corner1, y0); // dolny lewy
        vec3 v33 = mix(corner2, corner3, y1); // górny prawy
        vec3 v44 = mix(corner2, corner3, y0); // dolny prawy

        // dodanie wiatru
        v11 = ApplyWind(v11, y1);
        v22 = ApplyWind(v22, y0);
        v33 = ApplyWind(v33, y1);
        v44 = ApplyWind(v44, y0);

        // UV
        float uvLow  = float(i) / float(segments);
        float uvHigh = float(i + 1) / float(segments);

        vec2 quadTextCoord[4];
        quadTextCoord[0] = vec2(0.0, uvHigh);
        quadTextCoord[1] = vec2(0.0, uvLow);
        quadTextCoord[2] = vec2(1.0, uvHigh);
        quadTextCoord[3] = vec2(1.0, uvLow);

        // emitowanie wierzchołków
        CreateVertex(rotate_vector(rotation, v11), quadTextCoord[0]);
        CreateVertex(rotate_vector(rotation, v22), quadTextCoord[1]);
        CreateVertex(rotate_vector(rotation, v33), quadTextCoord[2]);
        CreateVertex(rotate_vector(rotation, v44), quadTextCoord[3]);
        EndPrimitive();
    }
}


void main(void)
{
    if (length(gs_in[0].worldPosition - perFrame.cameraPosition) < grassShaderBuffer.variables.x)
    {
        vec4 quaternion = CreateQuaternion(normalize(gs_in[0].normal));

        // XY quad (pionowy, klasyczny)
        CreateSegmentedQuad(
            quaternion,
            vec3(-0.5, 0.0, 0.0), vec3(-0.5, 1.0, 0.0), // dolny → górny lewy
            vec3( 0.5, 0.0, 0.0), vec3( 0.5, 1.0, 0.0)  // dolny → górny prawy
        );

        // ZY quad (prostopadły do XY)
        CreateSegmentedQuad(
            quaternion,
            vec3(0.0, 0.0, -0.5), vec3(0.0, 1.0, -0.5), // dolny → górny lewy
            vec3(0.0, 0.0,  0.5), vec3(0.0, 1.0,  0.5)  // dolny → górny prawy
        );

        // Dwa quady po przekątnej dla bliskich źdźbeł
        if (length(gs_in[0].worldPosition - perFrame.cameraPosition) < (grassShaderBuffer.variables.x) / 3.0)
        {
            // XYZ quad (przekątna)
            CreateSegmentedQuad(
                quaternion,
                vec3(-0.4, 0.0, -0.4), vec3(-0.4, 1.0, -0.4), // dolny → górny lewy
                vec3( 0.4, 0.0,  0.4), vec3( 0.4, 1.0,  0.4)  // dolny → górny prawy
            );

            // ZYX quad (druga przekątna)
            CreateSegmentedQuad(
                quaternion,
                vec3( 0.4, 0.0, -0.4), vec3( 0.4, 1.0, -0.4), // dolny → górny lewy
                vec3(-0.4, 0.0,  0.4), vec3(-0.4, 1.0,  0.4)  // dolny → górny prawy
            );
        }
    }
}