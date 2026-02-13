#version 450
#extension GL_GOOGLE_include_directive : enable
#include "../Common/PerFrameBuffer.glsl"

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

const int gridWidth = 20;
const int gridHeight = 20;
const float sizeX = 2.0;
const float sizeY = 1.5;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

void main()
{
    int quadID = gl_VertexID / 6;
    int vertID = gl_VertexID % 6;

    int ix = quadID % (gridWidth - 1);
    int iy = quadID / (gridWidth - 1);

    ivec2 offsets[6] = ivec2[](
        ivec2(0, 0), ivec2(1, 0), ivec2(0, 1),
        ivec2(0, 1), ivec2(1, 0), ivec2(1, 1)
    );

    ivec2 currentVertex = ivec2(ix, iy) + offsets[vertID];

    vec2 uv = vec2(float(currentVertex.x) / float(gridWidth - 1),
                   float(currentVertex.y) / float(gridHeight - 1));

    vec3 pos = vec3(uv.x * sizeX, uv.y * sizeY, 0.0);

    float freq = 6.0;
    float speed = perFrame.time.x * 4.0;
    float amp = uv.x * 0.25;
    float wave = sin(uv.x * freq - speed + uv.y * 0.5);
    pos.z += wave * amp;

    float dv_dx = cos(uv.x * freq - speed + uv.y * 0.5) * freq * amp;
    vec3 tangent = vec3(1.0, 0.0, dv_dx);
    vec3 bitangent = vec3(0.0, 1.0, 0.0);
    vec3 newNormal = normalize(cross(tangent, bitangent));

    mat3 normalMatrix = transpose(inverse(mat3(perObjectUpdate.transformationMatrix)));
    Normal = normalMatrix * newNormal;

    vec4 worldPos = perObjectUpdate.transformationMatrix * vec4(pos, 1.0);
    FragPos = worldPos.xyz;

    gl_Position = perFrame.projectionViewMatrix * worldPos;
    TexCoord = uv;
}