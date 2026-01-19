#version 450 core

struct ClusterData
{
    vec4 center;     
    vec4 size;       
};

layout (std140, align=16, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
    vec4 projection;
} perFrame;

layout (std140, binding = 3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

layout(std430, binding = 5) readonly buffer ClusterBuffer
{
    ClusterData clusters[];
};

out VS_OUT
{
    vec2 texCoord;
    float layerIndex;
    vec4 worldPos;
} vs_out;

void main()
{
    int clusterIdx = gl_VertexID / 12;   
    int vertexIdx  = gl_VertexID % 12;
    int quadIdx    = vertexIdx / 6;      
    int triVertex  = vertexIdx % 6;     


    ClusterData cluster = clusters[clusterIdx];
    vec3 center = cluster.center.xyz;
    float scale = cluster.size.w; 

    vec2 posArr[6] = vec2[](
        vec2(-0.5, -0.5), vec2( 0.5, -0.5), vec2(-0.5,  0.5),
        vec2(-0.5,  0.5), vec2( 0.5, -0.5), vec2( 0.5,  0.5)
    );
    
    vec2 currentPos = posArr[triVertex];

    vec3 localPos;
    if (quadIdx == 0)
    {
        localPos = vec3(currentPos.x, currentPos.y, 0.0);
        vs_out.layerIndex = float(clusterIdx * 2);
    } else
    {
        localPos = vec3(0.0, currentPos.y, currentPos.x);
        vs_out.layerIndex = float(clusterIdx * 2 + 1);
    }

    vec3 worldPos = center + (localPos * scale);
    vs_out.texCoord = currentPos + 0.5; 
    vs_out.worldPos = vec4(worldPos, 1.0);
    
    gl_Position = perFrame.projectionViewMatrix * perObjectUpdate.transformationMatrix *  vec4(worldPos, 1.0);
}