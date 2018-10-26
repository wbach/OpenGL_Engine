#include "SkyBoxShader.h"
#include "glm/gtx/transform.hpp"

namespace GameEngine
{
SkyBoxShader::SkyBoxShader(IGraphicsApiPtr graphicsApi)
    : ShaderProgram(graphicsApi)
{
}

void SkyBoxShader::Init()
{
    rotation_      = 0.f;
    rotationSpeed_ = 0.1f;

    SetFiles({{"SkyBox/SkyboxVertexShader.vert", GameEngine::ShaderType::VERTEX_SHADER},
              {"SkyBox/SkyboxFragmentShader.frag", GameEngine::ShaderType::FRAGMENT_SHADER}});

    ShaderProgram::Init();
}

void SkyBoxShader::LoadProjectionMatrix(const mat4& matrix) const
{
    LoadValue(location_ProjectionMatrix, matrix);
}

void SkyBoxShader::LoadViewMatrix(mat4 matrix, const float& deltaTime, const float& distance_view)
{
    matrix[3][0] = 0;
    matrix[3][1] = 0;
    matrix[3][2] = 0;
    rotation_ += rotationSpeed_ * deltaTime;
    matrix *= glm::scale(vec3(distance_view * 0.5f));
    matrix *= glm::rotate((float)rotation_, .0f, 1.f, .0f);
    LoadValue(location_ViewMatrix, matrix);
}

void SkyBoxShader::GetAllUniformLocations()
{
    location_ProjectionMatrix = GetUniformLocation("ProjectionMatrix");
    location_ViewMatrix       = GetUniformLocation("ViewMatrix");
    location_DayCubeMap       = GetUniformLocation("DayCubeMap");
    location_NightCubeMap     = GetUniformLocation("NightCubeMap");
    location_FogColour        = GetUniformLocation("FogColour");
    location_BlendFactor      = GetUniformLocation("BlendFactor");
}

void SkyBoxShader::LoadFogColour(const float& r, const float& g, const float& b) const
{
    LoadValue(location_FogColour, vec3(r, g, b));
}

void SkyBoxShader::LoadBlendFactor(const float& factor) const
{
    LoadValue(location_BlendFactor, factor);
}

void SkyBoxShader::ConnectTextureUnits() const
{
    LoadValue(location_DayCubeMap, 0);
    LoadValue(location_NightCubeMap, 1);
}

void SkyBoxShader::BindAttributes() const
{
    BindAttribute(0, "position");
}
}  // namespace GameEngine
