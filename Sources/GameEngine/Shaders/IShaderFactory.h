#pragma once
#include <memory>
#include "GraphicsApi/ShadersTypes.h"

namespace GameEngine
{
class IShaderProgram;

class IShaderFactory
{
public:
    virtual ~IShaderFactory()
    {
    }
    virtual std::unique_ptr<IShaderProgram> create(GraphicsApi::Shaders) = 0;
};
}  // namespace GameEngine
