#pragma once
#include <GraphicsApi/IFrameBuffer.h>
#include <gmock/gmock.h>

namespace GraphicsApi
{
class FrameBufferMock : public IFrameBuffer
{
public:
    MOCK_METHOD0(Init, bool());
    MOCK_METHOD1(Bind, void(FrameBuffer::BindType));
    MOCK_METHOD0(UnBind, void());
    MOCK_METHOD0(Clear, void());
    MOCK_METHOD0(CleanUp, void());
    MOCK_CONST_METHOD1(GetAttachmentTexture, ID(FrameBuffer::Type));
    MOCK_METHOD1(TakeSnapshot, void(const std::string&));
    MOCK_METHOD(const vec2ui&, GetSize, (), (const, override));
};
}  // namespace GraphicsApi