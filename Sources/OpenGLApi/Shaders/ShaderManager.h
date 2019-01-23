#pragma once
#include "Types.h"
#include <optional>
#include "OpenGLShaderProgram.h"
#include "GraphicsApi/ShaderQuaility.h"
#include "GraphicsApi/ShaderTypes.h"
#include "GraphicsApi/ShadersTypes.h"
#include "GraphicsApi/FunctionsTypes.h"
#include "GraphicsApi/ShaderFiles.h"
#include <GL/glew.h>

namespace OpenGLApi
{
class IdPool;

class ShaderManager
{
public:
    ShaderManager(IdPool& idPool);
    void UseShader(uint32);
    void SetShadersFilesLocations(const std::string & path);
    uint32 Create(GraphicsApi::Shaders shaderType, GraphicsApi::GraphicsApiFunctions);
    void DeleteShader(uint32 programId);
    uint32 GetShaderVariableLocation(uint32, const std::string&);
    void SetShaderQuaility(GraphicsApi::ShaderQuaility);
    void BindAttribute(uint32, uint32, const std::string&);
    void LoadValueToShader(uint32, int);
    void LoadValueToShader(uint32, float);
    void LoadValueToShader(uint32, uint32);
    void LoadValueToShader(uint32, const vec2&);
    void LoadValueToShader(uint32, const vec3&);
    void LoadValueToShader(uint32, const vec4&);
    void LoadValueToShader(uint32, const mat3&);
    void LoadValueToShader(uint32, const mat4&);
    void LoadValueToShader(uint32, const std::vector<float>&);
    void LoadValueToShader(uint32, const std::vector<vec3>&);
    void LoadValueToShader(uint32, const std::vector<mat4>&);

private:
    std::optional<GLuint> CreateShaderProgram();
    bool AddShader(uint32 id, const std::string& filename, GraphicsApi::ShaderType mode);
    bool FinalizeShader(uint32 programId, GraphicsApi::GraphicsApiFunctions);
    GraphicsApi::ShadersFiles GetShaderFiles(GraphicsApi::Shaders shaderType);

private:
    IdPool& idPool_;
    std::string shadersFileLocation_;
    GraphicsApi::ShaderQuaility shaderQuality_;
    std::unordered_map<GLuint, OpenGLShaderProgram> shaderPrograms_;
};
} // namespace GameEngine
