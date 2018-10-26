#include "GrassShader.h"

namespace GameEngine
{
GrassShader::GrassShader(IGraphicsApiPtr graphicsApi)
    : ShaderProgram(graphicsApi)
{
}

void GrassShader::Init()
{
    SetFiles({{"Grass/GrassShader.vert", ShaderType::VERTEX_SHADER},
              {"Grass/GrassShader.frag", ShaderType::FRAGMENT_SHADER},
              {"Grass/GrassShader.geom", ShaderType::GEOMETRY_SHADER}});

    ShaderProgram::Init();
}

void GrassShader::LoadTransformMatrix(const mat4 &m) const
{
    LoadValue(location_TransformationMatrix, m);
}

void GrassShader::LoadProjectionMatrix(const mat4 &m) const
{
    LoadValue(location_ProjectionMatrix, m);
}

void GrassShader::LoadViewMatrix(const mat4 &m) const
{
    LoadValue(location_ViewMatrix, m);
}

void GrassShader::LoadGlobalTime(const float &time) const
{
    LoadValue(location_GlobalTime, time);
}

void GrassShader::LoadToShadowSpaceMatrix(const mat4 &matrix) const
{
    LoadValue(location_ToShadowMapSpace, matrix);
}

void GrassShader::LoadShadowValues(const float &is, const float &distance, const float &shadow_map_size) const
{
    LoadValue(location_ShadowVariables, vec3(is, distance - 5, shadow_map_size));
}
void GrassShader::LoadViewDistance(const float &distance) const
{
    LoadValue(location_ViewDistance, distance);
}
void GrassShader::ConnectTextureUnits() const
{
    LoadValue(location_Texture, 0);
    LoadValue(location_ShadowMap, 1);
}

void GrassShader::GetAllUniformLocations()
{
    location_ProjectionMatrix = GetUniformLocation("ProjectionMatrix");
    location_ViewMatrix       = GetUniformLocation("ViewMatrix");

    // Textures
    location_Texture = GetUniformLocation("Texture0");

    location_GlobalTime = GetUniformLocation("GlobalTime");

    // Shadows
    location_ShadowVariables  = GetUniformLocation("ShadowVariables");
    location_ShadowMap        = GetUniformLocation("ShadowMap");
    location_ToShadowMapSpace = GetUniformLocation("ToShadowMapSpace");

    // Skip render
    location_ViewDistance = GetUniformLocation("ViewDistance");
}

void GrassShader::BindAttributes()
{
    BindAttribute(0, "Position");
}
}  // namespace GameEngine
