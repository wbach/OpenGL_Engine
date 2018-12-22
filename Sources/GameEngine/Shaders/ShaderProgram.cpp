#include "ShaderProgram.h"
#include <fstream>
#include <string>
#include "../Engine/Configuration.h"
#include "Logger/Log.h"
#include "glm/gtc/type_ptr.hpp"

namespace GameEngine
{
ShaderProgram::ShaderProgram(GameEngine::IGraphicsApiPtr graphicsApi, const ShadersFiles& files)
    : graphicsApi_(graphicsApi)
    , programID_(0)
    , shaderFiles_(files)
{
}

ShaderProgram::~ShaderProgram()
{
    Clear();
}
void ShaderProgram::Init()
{
    GameEngine::GraphicsApiFunctions f;
    f[GameEngine::GraphicFunctionType::SHADER_SET_ID] =
        std::bind(&ShaderProgram::SetProgramId, this, std::placeholders::_1);

    f[GameEngine::GraphicFunctionType::SHADER_BIND_ATTRIBUTES] =
        std::bind(&ShaderProgram::BindAttributesFunction, this, std::placeholders::_1);

    f[GameEngine::GraphicFunctionType::SHADER_VARIABLES_LOCATION] =
        std::bind(&ShaderProgram::GetAllUniformLocationsFunction, this, std::placeholders::_1);

    f[GameEngine::GraphicFunctionType::SHADER_CONNECT_TEXTURES] =
        std::bind(&ShaderProgram::ConnectTextureUnitsFunction, this, std::placeholders::_1);

    programID_ = graphicsApi_->CreateShader(shaderFiles_, f);
}
void ShaderProgram::SetFiles(const GameEngine::ShadersFiles& files)
{
    shaderFiles_ = files;
}
void ShaderProgram::Reload()
{
    if (!IsReadyToLoad())
        return;

    Log("Reload shader: " + name_);

    Clear();
    Init();
}
bool ShaderProgram::IsReady() const
{
    return programID_ != 0;
}
bool ShaderProgram::IsReadyToLoad() const
{
    return !shaderFiles_.empty();
}

void ShaderProgram::Start() const
{
    graphicsApi_->UseShader(programID_);
}
void ShaderProgram::Stop() const
{
    graphicsApi_->UseShader(0);
}
uint32 ShaderProgram::GetUniformLocation(const std::string& uniformName) const
{
    return graphicsApi_->GetShaderVariableLocation(programID_, uniformName.c_str());
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
    graphicsApi_->DeleteObject(programID_);
}
void ShaderProgram::BindAttribute(int attribute, const std::string& variableName) const
{
    graphicsApi_->BindAttribute(programID_, attribute, variableName);
}
void ShaderProgram::Load(uint32 loacation, uint32 value) const
{
    graphicsApi_->LoadValueToShader(uniforms_[loacation], value);
}
void ShaderProgram::Load(uint32 loacation, const mat4& value) const
{
    graphicsApi_->LoadValueToShader(uniforms_[loacation], value);
}
void ShaderProgram::Load(uint32 loacation, const std::vector<float>& value) const
{
     graphicsApi_->LoadValueToShader(uniforms_[loacation], value);
}
void ShaderProgram::Load(uint32 loacation, const std::vector<vec3>& value) const
{
     graphicsApi_->LoadValueToShader(uniforms_[loacation], value);
}
void ShaderProgram::Load(uint32 loacation, const mat3& value) const
{
     graphicsApi_->LoadValueToShader(uniforms_[loacation], value);
}

void ShaderProgram::Load(uint32 loacation, float value) const
{
     graphicsApi_->LoadValueToShader(uniforms_[loacation], value);
}

void ShaderProgram::Load(uint32 loacation, const vec2ui& value) const
{
    graphicsApi_->LoadValueToShader(uniforms_[loacation], vec2(value.x, value.y));
}

void ShaderProgram::Load(uint32 varIndex, bool value) const
{
    graphicsApi_->LoadValueToShader(uniforms_[varIndex], value ? 1.f : 0.f);
}

void ShaderProgram::Load(uint32 loacation, int value) const
{
     graphicsApi_->LoadValueToShader(uniforms_[loacation], value);
}

void ShaderProgram::Load(uint32 loacation, const vec2& value) const
{
     graphicsApi_->LoadValueToShader(uniforms_[loacation], value);
}

void ShaderProgram::Load(uint32 loacation, const vec3& value) const
{
     graphicsApi_->LoadValueToShader(uniforms_[loacation], value);
}

void ShaderProgram::Load(uint32 loacation, const vec4& value) const
{
     graphicsApi_->LoadValueToShader(uniforms_[loacation], value);
}
}  // namespace GameEngine
