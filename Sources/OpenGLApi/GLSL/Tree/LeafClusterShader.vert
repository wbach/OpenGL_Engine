#version 450 core

struct ClusterData
{
    vec4 center;
    vec4 size;
};

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
    vec4 time;
} perFrame;

layout (std140, binding = 3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

layout(std140, align=16, binding = 4) uniform LeafParams
{
    vec4 wind;
    vec4 fparams;
    ivec4 atlasParams;
    float time;
} leafParams;

layout(std430, binding = 5) readonly buffer ClusterBuffer
{
    ClusterData clusters[];
};

out VS_OUT
{
    vec2 texCoord;
    float layerIndex;
    vec4 worldPos;
    float visibility;
} vs_out;

void main()
{
    int clusterIdx = gl_VertexID / 12;
    int vertexIdx  = gl_VertexID % 12;
    int quadIdx    = vertexIdx / 6;
    int triVertex  = vertexIdx % 6;

    ClusterData cluster = clusters[clusterIdx];
    vec3 center = cluster.center.xyz;

    vec2 posArr[6] = vec2[](
        vec2(-0.5, -0.5), vec2( 0.5, -0.5), vec2(-0.5,  0.5),
        vec2(-0.5,  0.5), vec2( 0.5, -0.5), vec2( 0.5,  0.5)
    );

    vec2 currentPos = posArr[triVertex];
    vec3 localScaled;

    if (quadIdx == 0)
        localScaled = vec3(currentPos.x * cluster.size.x, currentPos.y * cluster.size.y, 0.0);
    else
        localScaled = vec3(0.0, currentPos.y * cluster.size.y, -currentPos.x * cluster.size.z);

    float windStrength = leafParams.wind.w;
    vec3 windDir = leafParams.wind.xyz;
    float phase = dot(center, vec3(0.1));
    float wave = sin(leafParams.time * 1.5 + phase) * cos(leafParams.time * 0.7 + phase);
    vec3 windOffset = windDir * wave * windStrength * 0.15;

    float vertexNoise = sin(leafParams.time * 5.0 + currentPos.x + currentPos.y);
    vec3 microWiggle = vec3(0.0, vertexNoise * 0.03 * windStrength, 0.0);

    vec3 worldPos = center + localScaled + windOffset + microWiggle;

    vs_out.texCoord = currentPos + 0.5;
    vs_out.worldPos = vec4(worldPos, 1.0);

    if (quadIdx == 0)
        vs_out.layerIndex = float(clusterIdx * 2);
    else
        vs_out.layerIndex = float(clusterIdx * 2 + 1);


    vec4 worldPosition = perObjectUpdate.transformationMatrix * vec4(worldPos, 1.0);

    float distance = length(perFrame.cameraPosition - worldPosition.xyz);
    float visibility = exp(-pow((distance*( ( 3.0f / perApp.viewDistance.x))), perApp.fogData.w));
    vs_out.visibility = clamp(visibility, 0.f, 1.f);

    gl_Position = perFrame.projectionViewMatrix * worldPosition;
}
