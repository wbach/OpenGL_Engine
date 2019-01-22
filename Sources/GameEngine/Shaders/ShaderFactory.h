#pragma once
#include "GraphicsApi/IGraphicsApi.h"
#include "GraphicsApi/ShadersTypes.h"
#include "IShaderFactory.h"

namespace GameEngine
{
class IShaderProgram;

class ShaderFactory : public IShaderFactory
{
public:
    ShaderFactory(GraphicsApi::IGraphicsApi& graphicsApi);
    virtual std::unique_ptr<IShaderProgram> create(GraphicsApi::Shaders) override;

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
};
}  // namespace GameEngine