#pragma once
#include "Utils/Utils.h"
#include "GameEngine/Api/IGraphicsApi.h"

class CShadowFrameBuffer
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
    vec2ui windowSize;

    bool isInit = false;
};
