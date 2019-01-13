#pragma once
#include "../FrameBuffer.h"

namespace GameEngine
{
class PostprocessFrameBuffer : public FrameBuffer
{
public:
    PostprocessFrameBuffer(IGraphicsApi& api);
    void Init(const wb::vec2ui& size) override;
    void Clean() override;
};
}  // GameEngine
