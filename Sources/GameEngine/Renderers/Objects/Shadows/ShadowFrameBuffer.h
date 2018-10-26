#pragma once
#include "GameEngine/Api/IGraphicsApi.h"
#include "IShadowFrameBuffer.hpp"
#include "Utils/Utils.h"

namespace GameEngine
{
class ShadowFrameBuffer : public IShadowFrameBuffer
{
public:
    ShadowFrameBuffer(IGraphicsApiPtr graphicsApi);
    virtual ~ShadowFrameBuffer();

    void BindFBO();
    void UnbindFrameBuffer() const;
    void InitialiseFrameBuffer();
    uint32 GetShadowMap() const;

private:
    IGraphicsApiPtr graphicsApi_;
    uint32 fbo;
    uint32 shadowMap;

    vec2ui size;
    vec2ui renderResolution;

    bool isInit = false;
};
}  // namespace GameEngine
