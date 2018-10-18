#pragma once
#include <gmock/gmock.h>
#include "GameEngine/Renderers/Framebuffer/IFrameBuffer.h"
#include "Types.h"

namespace GameEngine
{
class FrameBufferMock : public IFrameBuffer
{
public:
    MOCK_METHOD1(Init, void(const vec2ui&));
    MOCK_METHOD0(Clean, void());
    MOCK_METHOD0(CreateFrameBuffer, void());
    MOCK_METHOD1(AddTexture, void(uint32 texture));
    MOCK_METHOD1(SetDepthTexture, void(uint32 texture));
    MOCK_METHOD0(CheckStatus, int());
    MOCK_METHOD0(GetFbo, uint32());
    MOCK_METHOD0(GetDepthTexture, uint32());
    MOCK_METHOD1(GetTexture, uint32(const uint32& id));
    MOCK_METHOD1(BindTextures, void(int offset));
    MOCK_METHOD0(BindToDraw, void());
    MOCK_METHOD0(Bind, void());
    MOCK_METHOD0(UnBind, void());
    MOCK_METHOD0(UnBindDraw, void());
};
}  // GameEngine
