#version 440 core

const vec3 WORLD_UP = vec3(0, 1, 0);

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

layout (std140, align=16, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
    vec4 projection;
} perFrame;

layout(std140, align=16, binding = 4) uniform LeafParams
{
    vec4 fparams; // x - leafScale, y - leafOffset
    ivec4 atlasParams; // x - atlasSize, y - atlasIndex
} leafParams;

in VS_OUT
{
    vec3 worldPosition;
    vec3 worldDirection; // kierunek liścia w stronę gałęzi
} gs_in[];

out GS_OUT
{
    vec2 texCoord;
    vec4 worldPos;
    vec3 normal;
    vec3 color;
} gs_out;

void EmitLeafVertex(vec3 pos, vec3 normal, vec2 uv)
{
    gl_Position = perFrame.projectionViewMatrix * vec4(pos, 1.0);
    gs_out.texCoord = uv;
    gs_out.worldPos  = vec4(pos,1.0);
    gs_out.normal    = normalize(normal);
    gs_out.color     = vec3(1.0);
    EmitVertex();
}

void main()
{
    float scale      = leafParams.fparams.x;
    float leafOffset = leafParams.fparams.y;
    float bendAmount = leafParams.fparams.z;

    // Pozycja gałęzi (dolny środek liścia)
    vec3 leafBase = gs_in[0].worldPosition + normalize(gs_in[0].worldDirection) * leafOffset;
    vec3 leafDir  = normalize(gs_in[0].worldDirection);

    // wektory lokalne liścia
    vec3 right = -normalize(cross(leafDir, vec3(0.0, 1.0, 0.0))) * scale;
    vec3 up    = normalize(cross(right, leafDir)) * scale;

    // lekkie wygięcie liścia wzdłuż gałęzi
    vec3 bend = leafDir * bendAmount;

    // wierzchołki quada (dolny środek przy gałęzi)
    vec3 verts[4];
    verts[0] = leafBase - right;             // dolny-lewy
    verts[1] = leafBase + right;             // dolny-prawy
    verts[2] = leafBase - right + up + bend; // górny-lewy
    verts[3] = leafBase + right + up + bend; // górny-prawy

    vec2 uvs[4] = vec2[](vec2(0,0), vec2(1,0), vec2(0,1), vec2(1,1));

    for(int i = 0; i < 4; i++)
        EmitLeafVertex(verts[i], leafDir, uvs[i]);

    EndPrimitive();
}
