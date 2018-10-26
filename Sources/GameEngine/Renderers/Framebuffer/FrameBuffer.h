#pragma once
#include <vector>
#include "GameEngine/Api/IGraphicsApi.h"
#include "IFrameBuffer.h"
#include "Types.h"

namespace GameEngine
{
class FrameBuffer : public IFrameBuffer
{
public:
    FrameBuffer(IGraphicsApiPtr api);
    virtual void Init(const wb::vec2ui& size) = 0;
    virtual void Clean() override
    {
    }
    void CreateFrameBuffer() final;
    void AddTexture(uint32 texture) final;
    void SetDepthTexture(uint32 texture) final;
    int CheckStatus() final;
    uint32 GetFbo() final;
    uint32 GetDepthTexture() final;
    uint32 GetTexture(const uint32& id) final;
    void BindTextures(int offset = 0) final;
    void BindToDraw() final;
    void Bind() final;
    void UnBind() final;
    void UnBindDraw() final;
    virtual ~FrameBuffer() override;

protected:
    void CleanTexures();

protected:
    IGraphicsApiPtr graphicsApi_;
    bool isInitialized = false;
    uint32 fbo;
    uint32 depthTexture;
    std::vector<uint32> textures;
    bool depthStorage = false;
};
} //namespace GameEngine
