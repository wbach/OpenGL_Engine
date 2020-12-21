
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
    ShaderProgram(GraphicsApi::IGraphicsApi&, GraphicsApi::ShaderProgramType);
    virtual ~ShaderProgram();

    void Init() override;
    void Reload() override;
    void Clear() override;
    bool IsReady() const override;
    void Start() const override;
    void Stop() const override;

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    GraphicsApi::ID programID_;
    GraphicsApi::ShaderProgramType shaderType_;
};
}  // namespace GameEngine
