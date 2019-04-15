#include "ShaderProgram.h"
#include <fstream>
#include <string>
#include "../Engine/Configuration.h"
#include "Logger/Log.h"
#include "glm/gtc/type_ptr.hpp"

namespace GameEngine
{
ShaderProgram::ShaderProgram(GraphicsApi::IGraphicsApi& graphicsApi, GraphicsApi::Shaders shaderType)
    : graphicsApi_(graphicsApi)
    , programID_(0)
    , shaderType_(shaderType)
{
}

ShaderProgram::~ShaderProgram()
{
    Clear();
}
bool ShaderProgram::Init()
{
    GraphicsApi::GraphicsApiFunctions f;
    f[GraphicsApi::GraphicFunctionType::SHADER_SET_ID] =
        std::bind(&ShaderProgram::SetProgramId, this, std::placeholders::_1);

    f[GraphicsApi::GraphicFunctionType::SHADER_BIND_ATTRIBUTES] =
        std::bind(&ShaderProgram::BindAttributesFunction, this, std::placeholders::_1);

    f[GraphicsApi::GraphicFunctionType::SHADER_VARIABLES_LOCATION] =
        std::bind(&ShaderProgram::GetAllUniformLocationsFunction, this, std::placeholders::_1);

    f[GraphicsApi::GraphicFunctionType::SHADER_CONNECT_TEXTURES] =
        std::bind(&ShaderProgram::ConnectTextureUnitsFunction, this, std::placeholders::_1);

    programID_ = graphicsApi_.CreateShader(shaderType_, f);

    return programID_ != 0;
}
void ShaderProgram::Reload()
{
    if (!IsReadyToLoad())
        return;

    DEBUG_LOG("Reload shader: " + name_);

    Clear();
    Init();
}
bool ShaderProgram::IsReady() const
{
    return programID_ != 0;
}
bool ShaderProgram::IsReadyToLoad() const
{
    return true;
}

void ShaderProgram::Start() const
{
    graphicsApi_.UseShader(programID_);
}
void ShaderProgram::Stop() const
{
    graphicsApi_.UseShader(0);
}
uint32 ShaderProgram::GetUniformLocation(const std::string& uniformName) const
{
    return graphicsApi_.GetShaderVariableLocation(programID_, uniformName.c_str());
}
bool ShaderProgram::validUniformLocation(uint32 location) const
{
    return location < uniforms_.size();
}
void ShaderProgram::SetProgramId(uint32 id)
{
    programID_ = id;
}
void ShaderProgram::GetAllUniformLocationsFunction(uint32)
{
    GetAllUniformLocations();
}
void ShaderProgram::BindAttributesFunction(uint32)
{
    BindAttributes();
}
void ShaderProgram::ConnectTextureUnitsFunction(uint32)
{
    ConnectTextureUnits();
}
void ShaderProgram::Clear()
{
    graphicsApi_.DeleteObject(programID_);
}
void ShaderProgram::BindAttribute(int attribute, const std::string& variableName) const
{
    graphicsApi_.BindAttribute(programID_, attribute, variableName);
}
void ShaderProgram::Load(uint32 loacation, uint32 value) const
{
    if (validUniformLocation(loacation))
        graphicsApi_.LoadValueToShader(uniforms_[loacation], value);
}
void ShaderProgram::Load(uint32 loacation, const mat4& value) const
{
    if (validUniformLocation(loacation))
        graphicsApi_.LoadValueToShader(uniforms_[loacation], value);
}
void ShaderProgram::Load(uint32 loacation, const std::vector<float>& value) const
{
    if (validUniformLocation(loacation))
        graphicsApi_.LoadValueToShader(uniforms_[loacation], value);
}
void ShaderProgram::Load(uint32 loacation, const std::vector<vec3>& value) const
{
    if (validUniformLocation(loacation))
        graphicsApi_.LoadValueToShader(uniforms_[loacation], value);
}
void ShaderProgram::Load(uint32 loacation, const std::vector<mat4>& value) const
{
    if (validUniformLocation(loacation))
        graphicsApi_.LoadValueToShader(uniforms_[loacation], value);
}
void ShaderProgram::Load(uint32 loacation, const mat3& value) const
{
    if (validUniformLocation(loacation))
        graphicsApi_.LoadValueToShader(uniforms_[loacation], value);
}

void ShaderProgram::Load(uint32 loacation, float value) const
{
    if (validUniformLocation(loacation))
        graphicsApi_.LoadValueToShader(uniforms_[loacation], value);
}

void ShaderProgram::Load(uint32 loacation, const vec2ui& value) const
{
    if (validUniformLocation(loacation))
        graphicsApi_.LoadValueToShader(uniforms_[loacation], vec2(value.x, value.y));
}

void ShaderProgram::Load(uint32 loacation, bool value) const
{
    if (validUniformLocation(loacation))
        graphicsApi_.LoadValueToShader(uniforms_[loacation], value ? 1.f : 0.f);
}

void ShaderProgram::Load(uint32 loacation, int value) const
{
    if (validUniformLocation(loacation))
        graphicsApi_.LoadValueToShader(uniforms_[loacation], value);
}

void ShaderProgram::Load(uint32 loacation, const vec2& value) const
{
    if (validUniformLocation(loacation))
        graphicsApi_.LoadValueToShader(uniforms_[loacation], value);
}

void ShaderProgram::Load(uint32 loacation, const vec3& value) const
{
    if (validUniformLocation(loacation))
        graphicsApi_.LoadValueToShader(uniforms_[loacation], value);
}

void ShaderProgram::Load(uint32 loacation, const vec4& value) const
{
    if (validUniformLocation(loacation))
        graphicsApi_.LoadValueToShader(uniforms_[loacation], value);
}
}  // namespace GameEngine
