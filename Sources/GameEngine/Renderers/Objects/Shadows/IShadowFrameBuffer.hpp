#pragma once
#include "Types.h"

namespace GameEngine
{
class IShadowFrameBuffer
{
public:
    virtual ~IShadowFrameBuffer()
    {
    }
    virtual void BindFBO()                 = 0;
    virtual void UnbindFrameBuffer() const = 0;
    virtual void InitialiseFrameBuffer()   = 0;
    virtual uint32 GetShadowMap() const    = 0;
};
} // GameEngine
