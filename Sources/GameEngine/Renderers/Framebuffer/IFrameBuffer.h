#pragma once
#include "Types.h"

namespace GameEngine
{
class IFrameBuffer
{
public:
    virtual ~IFrameBuffer()
    {
    }
    virtual void Init(const vec2ui& size)        = 0;
    virtual void Clean()                         = 0;
    virtual void CreateFrameBuffer()             = 0;
    virtual void AddTexture(uint32 texture)      = 0;
    virtual void SetDepthTexture(uint32 texture) = 0;
    virtual int CheckStatus()                    = 0;
    virtual uint32 GetFbo()                      = 0;
    virtual uint32 GetDepthTexture()             = 0;
    virtual uint32 GetTexture(const uint32& id)  = 0;
    virtual void BindTextures(int offset)        = 0;
    virtual void BindToDraw()                    = 0;
    virtual void Bind()                          = 0;
    virtual void UnBind()                        = 0;
    virtual void UnBindDraw()                    = 0;
};
}  // GameEngine
