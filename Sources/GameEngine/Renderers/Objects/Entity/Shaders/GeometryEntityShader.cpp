#include "GeometryEntityShader.h"

namespace GameEngine
{
EntityGeometryPassShader::EntityGeometryPassShader(IGraphicsApiPtr graphicsApi)
    : EntityShader(graphicsApi)
{
}

void EntityGeometryPassShader::GetAllUniformLocations()
{
    EntityShader::GetAllUniformLocations();

    location_ClipPlane = GetUniformLocation("ClipPlane");

    location_UseFakeLighting = GetUniformLocation("IsUseFakeLighting");
    location_UseNormalMap    = GetUniformLocation("IsUseNormalMap");
    // Textures
    location_UseTexture   = GetUniformLocation("UseTexture");
    location_ModelTexture = GetUniformLocation("gColorMap");
    location_NormalMap    = GetUniformLocation("NormalMap");

    // Shadows
    location_ShadowVariables  = GetUniformLocation("ShadowVariables");
    location_ShadowMap        = GetUniformLocation("ShadowMap");
    location_ToShadowMapSpace = GetUniformLocation("ToShadowMapSpace");

    // Mesh Material
    location_MaterialAmbient  = GetUniformLocation("ModelMaterial.m_Ambient");
    location_MaterialDiffuse  = GetUniformLocation("ModelMaterial.m_Diffuse");
    location_MaterialSpecular = GetUniformLocation("ModelMaterial.m_Specular");

    // Skip render
    location_ViewDistance = GetUniformLocation("ViewDistance");
}

void EntityGeometryPassShader::ConnectTextureUnits() const
{
    LoadValue(location_ModelTexture, 0);
    LoadValue(location_NormalMap, 2);
    LoadValue(location_ShadowMap, 3);
}

void EntityGeometryPassShader::BindAttributes()
{
    BindAttribute(0, "Position");
    BindAttribute(1, "TexCoord");
    BindAttribute(2, "Normal");
    BindAttribute(3, "Tangent");
    BindAttribute(4, "TransformationMatrixes");
}

void EntityGeometryPassShader::Init()
{
    SetFiles({{"Entity/EntityGeometryPassShader.vert", ShaderType::VERTEX_SHADER},
              {"Entity/EntityGeometryPassShader.frag", ShaderType::FRAGMENT_SHADER}});

    ShaderProgram::Init();
}

void EntityGeometryPassShader::LoadViewDistance(const float& distance) const
{
    LoadValue(location_ViewDistance, distance);
}

void EntityGeometryPassShader::LoadUseFakeLight(const float& use) const
{
    LoadValue(location_UseFakeLighting, use);
}

void EntityGeometryPassShader::LoadUseNormalMap(const float& use) const
{
    LoadValue(location_UseNormalMap, use);
}

void EntityGeometryPassShader::LoadMeshMaterial(const Material &material) const
{
    LoadValue(location_MaterialAmbient, material.ambient);
    LoadValue(location_MaterialDiffuse, material.diffuse);
    LoadValue(location_MaterialSpecular, material.specular);
}

void EntityGeometryPassShader::LoadToShadowSpaceMatrix(const mat4& matrix) const
{
    LoadValue(location_ToShadowMapSpace, matrix);
}

void EntityGeometryPassShader::LoadShadowValues(const float& is, const float& distance,
                                                const float& shadow_map_size) const
{
    LoadValue(location_ShadowVariables, vec3(is, distance - 5, shadow_map_size));
}

void EntityGeometryPassShader::LoadClipPlane(const vec4 clip_plane) const
{
    LoadValue(location_ClipPlane, clip_plane);
}

void EntityGeometryPassShader::LoadUseTexture(const float& is) const
{
    LoadValue(location_UseTexture, is);
}
}  // namespace GameEngine
