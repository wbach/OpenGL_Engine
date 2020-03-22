#pragma once
#include <GL/glew.h>
#include <GraphicsApi/FunctionsTypes.h>
#include <GraphicsApi/GraphicsApiDef.h>
#include <GraphicsApi/ShaderFiles.h>
#include <GraphicsApi/ShaderQuaility.h>
#include <GraphicsApi/ShaderTypes.h>
#include <GraphicsApi/ShaderProgramType.h>

#include <optional>

#include "OpenGLShaderProgram.h"
#include "Types.h"

namespace OpenGLApi
{
class IdPool;

class ShaderManager
{
public:
    ShaderManager(IdPool& idPool);
    void UseShader(uint32);
    void UseDeprectedShaders();
    void SetShadersFilesLocations(const std::string& path);
    GraphicsApi::ID Create(GraphicsApi::ShaderProgramType);
    void DeleteShader(uint32 programId);
    void SetShaderQuaility(GraphicsApi::ShaderQuaility);

private:
    uint32 GetBindedShader() const;

private:
    std::optional<GLuint> CreateShaderProgram();
    bool AddShader(OpenGLShaderProgram& shaderProgram, const std::string& filename, GraphicsApi::ShaderType mode);
    bool FinalizeShader(OpenGLShaderProgram& shaderProgram);
    GraphicsApi::ShadersFiles GetShaderFiles(GraphicsApi::ShaderProgramType);

private:
    bool useDeprectedShaders_;
    uint32 currentShader_;
    IdPool& idPool_;
    std::string shadersFileLocation_;
    GraphicsApi::ShaderQuaility shaderQuality_;
    std::unordered_map<GLuint, OpenGLShaderProgram> shaderPrograms_;
};
}  // namespace OpenGLApi
