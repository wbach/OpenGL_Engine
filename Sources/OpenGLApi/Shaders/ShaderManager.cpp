#include "ShaderManager.h"
#include <iostream>
#include <unordered_map>
#include "FullDefferedShaderFiles.h"
#include "Logger/Log.h"
#include "OpenGLApi/IdPool.h"
#include "SimpleDeprecetedShaders.h"
#include "SimpleForwardShaderFiles.h"
#include "Utils.h"
#include "glm/gtc/type_ptr.hpp"
namespace OpenGLApi
{
namespace
{
// clang-format off
const std::unordered_map<GraphicsApi::ShaderType, uint32> shaderTypeMap =
{
    {GraphicsApi::ShaderType::VERTEX_SHADER, GL_VERTEX_SHADER},
    {GraphicsApi::ShaderType::FRAGMENT_SHADER, GL_FRAGMENT_SHADER},
    {GraphicsApi::ShaderType::GEOMETRY_SHADER, GL_GEOMETRY_SHADER},
    {GraphicsApi::ShaderType::TESS_CONTROL_SHADER, GL_TESS_CONTROL_SHADER},
    {GraphicsApi::ShaderType::TESS_EVALUATION_SHADER, GL_TESS_EVALUATION_SHADER}
};
// clang-format on
}  // namespace

ShaderManager::ShaderManager(IdPool& idPool)
    : useDeprectedShaders_(false)
    , currentShader_(0)
    , idPool_(idPool)
    , shaderQuality_(GraphicsApi::ShaderQuaility::FullDefferedRendering)
{
}

void ShaderManager::UseDeprectedShaders()
{
    useDeprectedShaders_ = true;
}

void ShaderManager::UseShader(uint32 id)
{
    currentShader_ = id;
    auto glId = idPool_.ToGL(id);
    glUseProgram(glId);
}

void ShaderManager::SetShadersFilesLocations(const std::string& path)
{
    shadersFileLocation_ = path;
}

std::optional<GLuint> ShaderManager::CreateShaderProgram()
{
    auto id = glCreateProgram();
    if (id == 0)
    {
        Error("[Error] Error creating shader program.");
        return std::optional<GLuint>();
    }
    return id;
}

uint32 ShaderManager::Create(GraphicsApi::Shaders shaderType, GraphicsApi::GraphicsApiFunctions functions)
{
    auto files = GetShaderFiles(shaderType);

    auto program = CreateShaderProgram();

    if (!program)
        return 0;

    auto programId = *program;
    auto uId = idPool_.ToUint(programId);

    if (functions.count(GraphicsApi::GraphicFunctionType::SHADER_SET_ID) != 0)
        functions.at(GraphicsApi::GraphicFunctionType::SHADER_SET_ID)(uId);

    shaderPrograms_.insert({ uId, OpenGLShaderProgram(programId, files.begin()->first)});

    for (const auto& p : files)
    {
        if (!AddShader(shaderPrograms_.at(uId), p.first, p.second))
            return false;
    }

    if (!FinalizeShader(shaderPrograms_.at(uId), functions))
        return false;

    return uId;
}

bool ShaderManager::AddShader(OpenGLShaderProgram& shaderProgram, const std::string& filename, GraphicsApi::ShaderType mode)
{
    if (not shadersFileLocation_.empty() and shadersFileLocation_.back() != '/')
    {
        shadersFileLocation_ += '/';
    }

    auto full_path = shadersFileLocation_ + filename;

    std::string source = Utils::ReadFile(full_path);

    uint32 id;
    id = glCreateShader(shaderTypeMap.at(mode));

    if (id == 0)
    {
        Error("[Error] Error creating shader type " + shaderTypeMap.at(mode));
        return false;
    }

    shaderProgram.shaderObjectsList.push_back(id);

    const char* csource = source.c_str();

    glShaderSource(id, 1, &csource, NULL);

    glCompileShader(id);

    GLint compiled = GL_FALSE;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
        char err[1000];
        int length = 0;
        glGetShaderInfoLog(id, 1000, &length, err);
        Log("[Error] ERRORS in Shader! \nFile name:\t" + filename + "\nCompile status: \n\n" + err);
        return false;
    }
    if (id == GL_FALSE)
    {
    }
    glAttachShader(shaderProgram.id, id);

    return true;
}

bool ShaderManager::FinalizeShader(OpenGLShaderProgram& shaderProgram, GraphicsApi::GraphicsApiFunctions functions)
{
    if (functions.count(GraphicsApi::GraphicFunctionType::SHADER_BIND_ATTRIBUTES) != 0)
        functions.at(GraphicsApi::GraphicFunctionType::SHADER_BIND_ATTRIBUTES)(0);

    glLinkProgram(shaderProgram.id);

    GLint Success         = 0;
    GLchar ErrorLog[1024] = {0};

    glGetProgramiv(shaderProgram.id, GL_LINK_STATUS, &Success);
    if (Success == 0)
    {
        glGetProgramInfoLog(shaderProgram.id, sizeof(ErrorLog), NULL, ErrorLog);
        Error("Error linking shader program: " + shaderProgram.name + " : " + std::string(ErrorLog));
        return false;
    }

    glUseProgram(shaderProgram.id);

    if (functions.count(GraphicsApi::GraphicFunctionType::SHADER_VARIABLES_LOCATION) != 0)
        functions.at(GraphicsApi::GraphicFunctionType::SHADER_VARIABLES_LOCATION)(0);

    if (functions.count(GraphicsApi::GraphicFunctionType::SHADER_CONNECT_TEXTURES) != 0)
        functions.at(GraphicsApi::GraphicFunctionType::SHADER_CONNECT_TEXTURES)(0);

    glValidateProgram(shaderProgram.id);
    glGetProgramiv(shaderProgram.id, GL_VALIDATE_STATUS, &Success);

    if (!Success)
    {
        glGetProgramInfoLog(shaderProgram.id, sizeof(ErrorLog), NULL, ErrorLog);
        Error("Invalid shader program : " + shaderProgram.name + " : " + std::string(ErrorLog));
        return false;
    }

    for (auto& id : shaderProgram.shaderObjectsList)
        glDeleteShader(id);

    shaderProgram.shaderObjectsList.clear();

    auto glError = glGetError();
    if (glError != GL_NO_ERROR)
    {
        glGetProgramInfoLog(shaderProgram.id, sizeof(ErrorLog), NULL, ErrorLog);
        std::string error(ErrorLog);
        Error("GlError ShaderprogramID: : " + shaderProgram.name +
              " error code : " + std::to_string(glError) + " : ");
    }

    glUseProgram(0);

    return true;
}

void ShaderManager::DeleteShader(uint32 programId)
{
    auto glId = idPool_.ToGL(programId);

    for (auto& id : shaderPrograms_.at(programId).shaderObjectsList)
    {
        glDetachShader(glId, id);
        glDeleteShader(id);
    }

    glDeleteProgram(glId);

    shaderPrograms_.erase(programId);
}

void ShaderManager::BindAttribute(uint32 programId, uint32 attribute, const std::string& variableName)
{
    auto glId = idPool_.ToGL(programId);
    glBindAttribLocation(glId, attribute, variableName.c_str());
}

uint32 ShaderManager::GetShaderVariableLocation(uint32 id, const std::string& varname)
{
    auto glId = shaderPrograms_.at(id).id;
    auto i = glGetUniformLocation(glId, varname.c_str());

    if (i < 0)
    {
        Log("Variable \"" + varname + "\" not found " + " in : " + shaderPrograms_.at(id).name);
        return 0;
    }

    return static_cast<uint32>(i) + 1;
}

void ShaderManager::LoadValueToShader(uint32 loacation, int value)
{
    if (loacation)
    {
        glUniform1i(loacation - 1, value);
    }
}

void ShaderManager::LoadValueToShader(uint32 loacation, float value)
{
    if (loacation)
    {
        glUniform1f(loacation - 1, value);
    }
}

void ShaderManager::LoadValueToShader(uint32 loacation, uint32 value)
{
    if (loacation)
    {
        glUniform1ui(loacation - 1, value);
    }
}

void ShaderManager::LoadValueToShader(uint32 loacation, const vec2& value)
{
    if (loacation)
    {
        glUniform2fv(loacation - 1, 1, glm::value_ptr(value));
    }
}

void ShaderManager::LoadValueToShader(uint32 loacation, const vec3& value)
{
    if (loacation)
    {
        glUniform3fv(loacation - 1, 1, glm::value_ptr(value));
    }
}

void ShaderManager::LoadValueToShader(uint32 loacation, const vec4& value)
{
    if (loacation)
    {
        glUniform4fv(loacation - 1, 1, glm::value_ptr(value));
    }
}

void ShaderManager::LoadValueToShader(uint32 loacation, const mat3& value)
{
    if (loacation)
    {
        glUniformMatrix3fv(loacation - 1, 1, GL_FALSE, glm::value_ptr(value));
    }
}

void ShaderManager::LoadValueToShader(uint32 loacation, const mat4& value)
{
    if (loacation)
    {
        glUniformMatrix4fv(loacation - 1, 1, GL_FALSE, glm::value_ptr(value));
    }
}

void ShaderManager::LoadValueToShader(uint32 loacation, const std::vector<float>& v)
{
    if (loacation)
    {
        glUniform3fv(loacation - 1, v.size(), (const GLfloat*)&v[0]);
    }
}

void ShaderManager::LoadValueToShader(uint32 loacation, const std::vector<vec3>& v)
{
    if (loacation)
    {
        glUniform3fv(loacation - 1, v.size(), (const GLfloat*)&v[0]);
    }
}

void ShaderManager::LoadValueToShader(uint32 loacation, const std::vector<mat4>& v)
{
    if (loacation)
    {
        glUniformMatrix4fv(loacation - 1, v.size(), GL_FALSE, (const GLfloat*)&v[0]);
    }
}

uint32 ShaderManager::GetBindedShader() const
{
    return currentShader_;
}

void ShaderManager::SetShaderQuaility(GraphicsApi::ShaderQuaility q)
{
    shaderQuality_ = q;
}

GraphicsApi::ShadersFiles ShaderManager::GetShaderFiles(GraphicsApi::Shaders shaderType)
{
    if (useDeprectedShaders_)
    {
        return GetSimpleDeprecetedShaders(shaderType);
    }

    switch (shaderQuality_)
    {
        case GraphicsApi::ShaderQuaility::FullDefferedRendering:
            return GetFullDefferedShaderFiles(shaderType);
        case GraphicsApi::ShaderQuaility::SimpleForwardRendering:
            return GetSimpleForwardShaderFiles(shaderType);
    }

    return {};
}
}  // namespace OpenGLApi
