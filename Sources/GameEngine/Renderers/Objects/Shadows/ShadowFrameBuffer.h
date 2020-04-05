#pragma once
#include "IShadowFrameBuffer.hpp"
#include "Utils/Utils.h"

namespace GraphicsApi
{
class IGraphicsApi;
}  // namespace GraphicsApi
namespace GameEngine
{
class ShadowFrameBuffer : public IShadowFrameBuffer
{
public:
    ShadowFrameBuffer(GraphicsApi::IGraphicsApi& graphicsApi);
    virtual ~ShadowFrameBuffer();

    void BindFBO();
    void UnbindFrameBuffer() const;
    void InitialiseFrameBuffer();
    std::optional<uint32> GetShadowMap() const;

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    std::optional<uint32> fbo;
    std::optional<uint32> shadowMap;

    vec2ui size;
    vec2ui renderResolution;

    bool isInit = false;
};
}  // namespace GameEngine
