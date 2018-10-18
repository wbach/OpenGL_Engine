#pragma once
#include "IShadowFrameBuffer.hpp"
#include "GameEngine/Api/IGraphicsApi.h"
#include "Utils/Utils.h"

class CShadowFrameBuffer : public GameEngine::IShadowFrameBuffer
{
public:
    CShadowFrameBuffer(GameEngine::IGraphicsApiPtr graphicsApi);
    virtual ~CShadowFrameBuffer();

    void BindFBO();
    void UnbindFrameBuffer() const;
    void InitialiseFrameBuffer();
    uint32 GetShadowMap() const;

private:
    GameEngine::IGraphicsApiPtr graphicsApi_;
    uint32 fbo;
    uint32 shadowMap;

    vec2ui size;
    vec2ui renderResolution;

    bool isInit = false;
};
