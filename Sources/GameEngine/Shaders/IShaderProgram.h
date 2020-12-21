#pragma once
#include "GraphicsApi/IGraphicsApi.h"
#include "Types.h"

namespace GameEngine
{
class IShaderProgram
{
public:
    virtual ~IShaderProgram() = default;

    virtual void Init()   = 0;
    virtual void Reload() = 0;
    virtual void Clear() = 0;

    virtual bool IsReady() const = 0;
    virtual void Start() const   = 0;
    virtual void Stop() const    = 0;
};
}  // namespace GameEngine
