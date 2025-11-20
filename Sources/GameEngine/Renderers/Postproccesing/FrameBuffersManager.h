#pragma once
#include <Types.h>

namespace GraphicsApi
{
class IGraphicsApi;
class IFrameBuffer;
}  // namespace GraphicsApi

namespace GameEngine
{
class FrameBuffersManager
{
public:
    FrameBuffersManager(GraphicsApi::IGraphicsApi&, const vec2ui&);
    ~FrameBuffersManager();

    bool GetStatus() const;
    void BindForWriting();
    void BindForReading();
    void StartFrame();
    void Swap();

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    GraphicsApi::IFrameBuffer& fboA;
    GraphicsApi::IFrameBuffer& fboB;
    bool selectedA{true};
    bool status_;
};
}  // namespace GameEngine
