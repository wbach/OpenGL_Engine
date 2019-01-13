#pragma once
#include "IShaderFactory.h"
#include "GameEngine/Api/IGraphicsApi.h"

namespace GameEngine
{
class IShaderProgram;


class ShaderFactory : public IShaderFactory
{
public:
    ShaderFactory(IGraphicsApi& graphicsApi);
    virtual std::unique_ptr<IShaderProgram> create(Shaders) override;

private:
    IGraphicsApi& graphicsApi_;
};
}  // namespace GameEngine