#pragma once
#include <list>
#include <unordered_map>
#include "GraphicsApi/IGraphicsApi.h"
#include "IShaderProgram.h"
#include "Types.h"
#include "Utils.h"

const int MAX_BONES = 50;

namespace GameEngine
{
class ShaderProgram : public IShaderProgram
{
public:
    ShaderProgram(GraphicsApi::IGraphicsApi& graphicsApi, GraphicsApi::Shaders shaderType);
    virtual ~ShaderProgram();

    bool Init() override;
    void Reload() override;

    bool IsReady() const override;
    bool IsReadyToLoad() const override;

    void Start() const override;
    void Stop() const override;

    virtual void Load(uint32 varIndex, bool value) const;
    virtual void Load(uint32 varIndex, int value) const;
    virtual void Load(uint32 varIndex, uint32 value) const;
    virtual void Load(uint32 varIndex, float value) const;
    virtual void Load(uint32 varIndex, const vec2ui& value) const;
    virtual void Load(uint32 varIndex, const vec2& value) const;
    virtual void Load(uint32 varIndex, const vec3& value) const;
    virtual void Load(uint32 varIndex, const vec4& value) const;
    virtual void Load(uint32 varIndex, const mat3& value) const;
    virtual void Load(uint32 varIndex, const mat4& value) const;
    virtual void Load(uint32 varIndex, const std::vector<float>& value) const;
    virtual void Load(uint32 varIndex, const std::vector<vec3>& value) const;
    virtual void Load(uint32 varIndex, const std::vector<mat4>& value) const;

    uint32 GetUniformLocation(const std::string& uniformName) const;

protected:
    bool validUniformLocation(uint32 lcoation) const;
    virtual void GetAllUniformLocations()
    {
    }
    virtual void BindAttributes()
    {
    }
    virtual void ConnectTextureUnits() const
    {
    }
    void SetProgramId(uint32);
    void GetAllUniformLocationsFunction(uint32);
    void BindAttributesFunction(uint32);
    void ConnectTextureUnitsFunction(uint32);

    void Clear();
    void BindAttribute(int attribute, const std::string& variableName) const;

protected:
    std::vector<uint32> uniforms_;
    GraphicsApi::IGraphicsApi& graphicsApi_;

private:
    std::string name_;
    uint32 programID_;
    GraphicsApi::Shaders shaderType_;
};
}  // namespace GameEngine
