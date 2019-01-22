#pragma once
#include "../FrameBuffer.h"

namespace GameEngine
{
class DefferedFrameBuffer : public FrameBuffer
{
public:
    DefferedFrameBuffer(GraphicsApi::IGraphicsApi& api);
    void Init(const vec2ui& size) override;
    void Clean() override;
};
}  // namespace GameEngine
