#pragma once
#include "IShaderFactory.h"
#include "GameEngine/Api/IGraphicsApi.h"

namespace GameEngine
{
class IShaderProgram;


class ShaderFactory : public IShaderFactory
{
public:
    ShaderFactory(IGraphicsApiPtr graphicsApi);
    virtual std::unique_ptr<IShaderProgram> create(Shaders) override;

private:
    IGraphicsApiPtr graphicsApi_;
};
}  // namespace GameEngine