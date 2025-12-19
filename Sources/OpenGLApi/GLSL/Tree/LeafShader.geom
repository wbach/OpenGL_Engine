#version 440 core

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
    vec4 fparams; // x - leafScale, y - leafOffset, z - bendAmount
    ivec4 atlasParams; // x - atlasSize, y - atlasIndex
} leafParams;

in VS_OUT
{
    vec3 worldPosition;
    vec3 worldDirection;
    vec3 colorRandomness;
    float sizeRanfomness;
    int textureIndex;
} gs_in[];

out GS_OUT
{
    vec2 texCoord;
    vec4 worldPos;
    vec3 normal;
    vec3 colorRandomness;
    flat int textureIndex;
} gs_out;

// Emituje jeden wierzchołek liścia
void EmitLeafVertex(vec3 pos, vec3 normal, vec2 uv)
{
    gl_Position = perFrame.projectionViewMatrix * vec4(pos, 1.0);
    gs_out.texCoord             = uv;
    gs_out.worldPos             = vec4(pos,1.0);
    gs_out.normal               = normalize(normal);
    gs_out.colorRandomness      = gs_in[0].colorRandomness;
    gs_out.textureIndex         = gs_in[0].textureIndex;
    EmitVertex();
}

void main()
{
    float scale      = leafParams.fparams.x * gs_in[0].sizeRanfomness;
    float leafOffset = leafParams.fparams.y;
    float bendAmount = leafParams.fparams.z;

    // 1. Kierunek 'outward' (na zewnątrz od gałęzi) przekazany z CPU
    vec3 outwardDir = normalize(gs_in[0].worldDirection);
    
    // 2. Punkt bazowy liścia na powierzchni gałęzi
    vec3 leafBase = gs_in[0].worldPosition + outwardDir * leafOffset;

    // 3. Budujemy płaszczyznę liścia. 
    // Skoro outwardDir to kierunek "od gałęzi", to 'right' powinien być 
    // prostopadły zarówno do kierunku gałęzi jak i outwardDir.
    // Aby uniknąć skomplikowanych obliczeń, używamy triku z Cross:
    vec3 upRef = vec3(0.0, 1.0, 0.0); 
    vec3 right = normalize(cross(outwardDir, upRef)) * scale;
    
    // Jeśli liść patrzy prosto w górę, naprawiamy 'right'
    if(length(right) < 0.01) {
        right = normalize(cross(outwardDir, vec3(1,0,0))) * scale;
    }

    // 'up' to kierunek wzrostu liścia (wzdłuż outwardDir)
    vec3 up = outwardDir * scale;

    // 4. Bend (wygięcie) - dodajemy grawitację lub zakrzywienie
    // Przesuwamy tylko górne wierzchołki w dół (Y) lub w stronę gałęzi
    vec3 bend = vec3(0.0, -1.0, 0.0) * bendAmount; 

    // 5. Konstrukcja wierzchołków (Liść rośnie "od" leafBase)
    vec3 verts[4];
    // Dolna krawędź (przy gałęzi)
    verts[0] = leafBase - right * 0.5; 
    verts[1] = leafBase + right * 0.5;
    // Górna krawędź (koniec liścia) + bend
    verts[2] = leafBase - right * 0.5 + up + bend;
    verts[3] = leafBase + right * 0.5 + up + bend;

    // Normalna dla oświetlenia (płaszczyzna liścia)
    vec3 normal = normalize(cross(right, up));

    vec2 uvs[4] = vec2[](vec2(0,0), vec2(1,0), vec2(0,1), vec2(1,1));

    for(int i = 0; i < 4; i++)
        EmitLeafVertex(verts[i], normal, uvs[i]);

    EndPrimitive();
}

