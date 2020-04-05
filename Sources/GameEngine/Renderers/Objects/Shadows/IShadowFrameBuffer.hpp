#pragma once
#include "Types.h"
#include <optional>

namespace GameEngine
{
class IShadowFrameBuffer
{
public:
    virtual ~IShadowFrameBuffer() = default;
    virtual void BindFBO()                 = 0;
    virtual void UnbindFrameBuffer() const = 0;
    virtual void InitialiseFrameBuffer()   = 0;
    virtual std::optional<uint32> GetShadowMap() const    = 0;
};
} // GameEngine
