#include "ShaderProgram.h"
#include "../Engine/Configuration.h"
#include "Logger/Log.h"
#include "glm/gtc/type_ptr.hpp"
#include <string>
#include <fstream>

CShaderProgram::CShaderProgram(GameEngine::IGraphicsApiPtr graphicsApi)
	: graphicsApi_(graphicsApi)
	, programID_(0)
{
}

CShaderProgram::~CShaderProgram()
{
	Clear();
}
bool CShaderProgram::Init()
{
	GameEngine::GraphicsApiFunctions f;
	f[GameEngine::GraphicFunctionType::SHADER_SET_ID]				= std::bind(&CShaderProgram::SetProgramId, this, std::placeholders::_1);
	f[GameEngine::GraphicFunctionType::SHADER_BIND_ATTRIBUTES]		= std::bind(&CShaderProgram::BindAttributesFunction, this, std::placeholders::_1);
	f[GameEngine::GraphicFunctionType::SHADER_VARIABLES_LOCATION]	= std::bind(&CShaderProgram::GetAllUniformLocationsFunction, this, std::placeholders::_1);
	f[GameEngine::GraphicFunctionType::SHADER_CONNECT_TEXTURES]		= std::bind(&CShaderProgram::ConnectTextureUnitsFunction, this, std::placeholders::_1);
	programID_ = graphicsApi_->CreateShader(shaderFiles_, f);
	return true;
}
void CShaderProgram::SetFiles(const GameEngine::ShadersFiles & files)
{
	shaderFiles_ = files;
}
void CShaderProgram::Reload()
{
	if (!IsReadyToLoad())
		return;

	Log("Reload shader: " + name_);

	Clear();
	Init();
}
bool CShaderProgram::IsReady() const
{
	return programID_ != 0;
}
bool CShaderProgram::IsReadyToLoad() const
{
	return !shaderFiles_.empty();
}

void CShaderProgram::Start() const
{
	graphicsApi_->UseShader(programID_);
}
void CShaderProgram::Stop() const
{
	graphicsApi_->UseShader(0);
}
uint32 CShaderProgram::GetUniformLocation(const std::string& uniformName) const
{
	return graphicsApi_->GetShaderVariableLocation(programID_, uniformName.c_str());
}
void CShaderProgram::SetProgramId(uint32 id)
{
	programID_ = id;
}
void CShaderProgram::GetAllUniformLocationsFunction(uint32)
{
	GetAllUniformLocations();
}
void CShaderProgram::BindAttributesFunction(uint32)
{
	BindAttributes();
}
void CShaderProgram::ConnectTextureUnitsFunction(uint32)
{
	ConnectTextureUnits();
}
void CShaderProgram::Clear()
{
	graphicsApi_->DeleteObject(programID_);
}
void CShaderProgram::BindAttribute(int attribute, const std::string& variableName) const
{
	graphicsApi_->BindAttribute(programID_, attribute, variableName);
}
void CShaderProgram::LoadValue(uint32 loacation, uint32 value) const
{
	graphicsApi_->LoadValueToShader(loacation, value);
}
void CShaderProgram::LoadValue(uint32 loacation, const mat4& value) const
{
	graphicsApi_->LoadValueToShader(loacation, value);
}
void CShaderProgram::LoadValue(uint32 loacation, const mat3& value) const
{
	graphicsApi_->LoadValueToShader(loacation, value);
}

void CShaderProgram::LoadValue(uint32 loacation, float value) const
{
	graphicsApi_->LoadValueToShader(loacation, value);
}

void CShaderProgram::LoadValue(uint32 loacation, int value) const
{
	graphicsApi_->LoadValueToShader(loacation, value);
}

void CShaderProgram::LoadValue(uint32 loacation, const vec2& value) const
{
	graphicsApi_->LoadValueToShader(loacation, value);
}

void CShaderProgram::LoadValue(uint32 loacation, const vec3& value) const
{
	graphicsApi_->LoadValueToShader(loacation, value);
}

void CShaderProgram::LoadValue(uint32 loacation, const vec4& value) const
{
	graphicsApi_->LoadValueToShader(loacation, value);
}
