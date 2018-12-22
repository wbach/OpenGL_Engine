#pragma once
#include <memory>
#include "GameEngine/Api/ShadersTypes.h"

namespace GameEngine
{
class IShaderProgram;

class IShaderFactory
{
public:
    virtual ~IShaderFactory()
    {
    }
    virtual std::unique_ptr<IShaderProgram> create(Shaders) = 0;
};
}  // namespace GameEngine
