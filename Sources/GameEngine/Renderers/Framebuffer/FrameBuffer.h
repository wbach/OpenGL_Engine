#pragma once
#include "Types.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include <vector>

class CFrameBuffer
{
public:
	CFrameBuffer(GameEngine::IGraphicsApiPtr api);
    virtual void Init(const wb::vec2i& size) = 0;
    virtual void Clean(){}
    void CreateFrameBuffer();
    void AddTexture(uint32 texture);
    void SetDepthTexture(uint32 texture);
    int CheckStatus();
    uint32 GetFbo();
    uint32 GetDepthTexture();
    uint32 GetTexture(const uint32& id);
    void BindTextures(int offset = 0);
    void BindToDraw();
    void Bind();
    void UnBind();
    void UnBindDraw();
    virtual ~CFrameBuffer();

protected:
    void CleanTexures();

protected:
	GameEngine::IGraphicsApiPtr graphicsApi_;
    bool isInitialized = false;
	uint32 fbo;
	uint32 depthTexture;
    std::vector<uint32> textures;
    bool depthStorage = false;
};
