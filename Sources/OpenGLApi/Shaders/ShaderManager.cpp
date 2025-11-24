#include "ShaderManager.h"

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <filesystem>
#include <magic_enum/magic_enum.hpp>
#include <unordered_map>

#include "FullDefferedShaderFiles.h"
#include "Logger/Log.h"
#include "OpenGLApi/IdPool.h"
#include "OpenGLApi/OpenGLUtils.h"
#include "SimpleDeprecetedShaders.h"
#include "SimpleForwardShaderFiles.h"

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
    {GraphicsApi::ShaderType::TESS_EVALUATION_SHADER, GL_TESS_EVALUATION_SHADER},
    {GraphicsApi::ShaderType::COMPUTE_SHADER, GL_COMPUTE_SHADER}
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
    auto glId      = idPool_.ToGL(id);
    glUseProgram(glId);
}

void ShaderManager::SetShadersFilesLocations(const std::filesystem::path& path)
{
    shadersFileLocation_ = path / "OpenGLApi" / "GLSL";
}

std::optional<GLuint> ShaderManager::CreateShaderProgram()
{
    auto id = glCreateProgram();
    if (id == 0)
    {
        LOG_ERROR << "Error creating shader program.";
        return std::optional<GLuint>();
    }
    return id;
}

GraphicsApi::ID ShaderManager::Create(GraphicsApi::ShaderProgramType shaderType)
{
    LOG_DEBUG << "Create shader, type : " << magic_enum::enum_name(shaderType);

    auto files = GetShaderFiles(shaderType);
    if (not files)
    {
        LOG_ERROR << "Expected files not found for shader type " << magic_enum::enum_name(shaderType)
                  << " | Quality : " << magic_enum::enum_name(shaderQuality_);
        return std::nullopt;
    }

    std::string logFilesString;
    for (const auto& f : *files)
    {
        logFilesString += " " + f.second;
    }
    LOG_DEBUG << "Shader files :" << logFilesString;

    auto program = CreateShaderProgram();

    if (!program)
        return 0;

    auto programId = *program;
    auto uId       = idPool_.ToUint(programId);

    auto shaderName = std::filesystem::path(files->begin()->second).filename().replace_extension("").string();
    shaderPrograms_.insert({uId, OpenGLShaderProgram(programId, shaderName)});

    for (const auto& p : *files)
    {
        if (not AddShader(shaderPrograms_.at(uId), p.second, p.first))
            return {};
    }

    if (not FinalizeShader(shaderPrograms_.at(uId)))
        return {};

    LOG_DEBUG << "Shader succesful created. Id : " << uId;

    return uId;
}

bool ShaderManager::AddShader(OpenGLShaderProgram& shaderProgram, const std::string& filename, GraphicsApi::ShaderType mode)
{
    auto fullPath = shadersFileLocation_ / filename;

    std::string source = Utils::ReadFilesWithIncludes(fullPath.string());

    uint32 id;
    id = glCreateShader(shaderTypeMap.at(mode));

    if (id == 0)
    {
        CheckAndPrintGLError(shaderProgram);
        LOG_ERROR << "Error creating shader type " << static_cast<int>(shaderTypeMap.at(mode));
        return false;
    }

    shaderProgram.shaderObjectsList.push_back(id);

    const char* csource = source.c_str();

    glShaderSource(id, 1, &csource, nullptr);

    glCompileShader(id);

    GLint compiled = GL_FALSE;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);

    if (not compiled)
    {
        char err[1000];
        int length = 0;
        glGetShaderInfoLog(id, 1000, &length, err);
        LOG_ERROR << "ERRORS in Shader! \nFile name:\t" << filename << "\nCompile status: \n\n" << err;
        CheckAndPrintGLError(shaderProgram);
        return false;
    }
    if (id == GL_FALSE)
    {
    }
    glAttachShader(shaderProgram.id, id);

    return true;
}

bool ShaderManager::FinalizeShader(OpenGLShaderProgram& shaderProgram)
{
    glLinkProgram(shaderProgram.id);

    GLint success         = 0;
    GLchar ErrorLog[1024] = {0};

    glGetProgramiv(shaderProgram.id, GL_LINK_STATUS, &success);
    if (not success)
    {
        CheckAndPrintGLError(shaderProgram);
        glGetProgramInfoLog(shaderProgram.id, sizeof(ErrorLog), nullptr, ErrorLog);
        LOG_ERROR << "Error linking shader program: " << shaderProgram.name << " : " << ErrorLog;
        return false;
    }

    glUseProgram(shaderProgram.id);

    glValidateProgram(shaderProgram.id);
    glGetProgramiv(shaderProgram.id, GL_VALIDATE_STATUS, &success);

    if (not success)
    {
        CheckAndPrintGLError(shaderProgram);
        glGetProgramInfoLog(shaderProgram.id, sizeof(ErrorLog), nullptr, ErrorLog);
        LOG_ERROR << "Invalid shader program : " << shaderProgram.name << " : " << ErrorLog;
        return false;
    }

    for (auto& id : shaderProgram.shaderObjectsList)
        glDeleteShader(id);

    shaderProgram.shaderObjectsList.clear();
    CheckAndPrintGLError(shaderProgram);
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

uint32 ShaderManager::GetBindedShader() const
{
    return currentShader_;
}

void ShaderManager::SetShaderQuaility(GraphicsApi::ShaderQuaility q)
{
    shaderQuality_ = q;
}

void ShaderManager::CheckAndPrintGLError(OpenGLShaderProgram& shaderProgram)
{
    auto errorString = GetGlError();

    if (not errorString.empty())
    {
        LOG_ERROR << "GlError : " << errorString << ", Shaderprogram : " << shaderProgram.name << " (" << shaderProgram.id << ")";
    }
}

std::optional<GraphicsApi::ShadersFiles> ShaderManager::GetShaderFiles(GraphicsApi::ShaderProgramType shaderType)
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
