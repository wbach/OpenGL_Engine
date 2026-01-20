#version 450 core
#define EPSILON 0.001

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 MaterialSpecular;

layout(binding = 0) uniform sampler2DArray BaseColorTextureArray;
layout(binding = 1) uniform sampler2DArray NormalTextureArray;

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; 
    vec4 shadowVariables;
    vec4 fogData; 
} perApp;

layout (std140, align=16, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
    vec4 projection;
} perFrame;

layout (std140, align=16, binding=6) uniform PerMaterial
{
    vec4 baseColor;
    vec4 params; // x - metallicFactor, y - roughnessFactor, z - ambientOcclusion, w - opacityCutoff
    vec4 params2; // x - normalScale,  y - useFakeLighting, z - specularStrength, w - indexOfRefraction
    vec4 hasTextures; 
    vec4 hasTextures2; 
} perMaterial;

in VS_OUT
{
    vec2 texCoord;
    float layerIndex;
    vec4 worldPos;
} fs_in;

bool Is(float v)
{
    return v > 0.5f;
}

void main()
{
    vec3 uvl = vec3(fs_in.texCoord , fs_in.layerIndex);
    
    vec4 baseColor = vec4(1.0);
    vec3 normal = vec3(0.0, 1.0, 0.0); 
    
    if (Is(perApp.useTextures.x))
    {
        baseColor = texture(BaseColorTextureArray, uvl);
        
        if(baseColor.a < 0.5) discard; 
        baseColor.rgb *= vec3(0.95, 1.0, 0.95);
    }
    if (Is(perApp.useTextures.y))
    {
        normal = texture(NormalTextureArray, uvl).xyz * 2.0 - 1.0;

        vec3 viewDir = normalize(perFrame.cameraPosition - fs_in.worldPos.xyz);
        if (dot(normal, viewDir) < 0.0)
        {
            discard;//normal = -normal;
        }
        
        vec3 sphereNormal = normalize(fs_in.worldPos.xyz - (fs_in.worldPos.xyz - vec3(0, 2, 0)));
        normal = normalize(mix(normal, sphereNormal, 0.6));
        normal = normalize(normal);
    }


    float roughness = perMaterial.params.y; // 1.0
    float metallic  = perMaterial.params.x; // 0.0

    WorldPosOut      = fs_in.worldPos;
    DiffuseOut       = baseColor;
    NormalOut        = vec4(normal, 1.0);
    MaterialSpecular = vec4(metallic, roughness, 0.0, 1.0);
}
