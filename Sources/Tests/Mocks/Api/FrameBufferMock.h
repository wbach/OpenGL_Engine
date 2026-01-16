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
    MOCK_METHOD(void, BindTexture, (IdType, FrameBuffer::Type), (override));
    MOCK_METHOD(void, BindTextureLayer, (IdType, FrameBuffer::Type, int), (override));
    MOCK_METHOD0(UnBind, void());
    MOCK_METHOD0(Clear, void());
    MOCK_METHOD0(CleanUp, void());
    MOCK_CONST_METHOD1(GetAttachmentTexture, ID(FrameBuffer::Type));
    MOCK_METHOD1(TakeSnapshot, void(const std::filesystem::path&));
    MOCK_METHOD(const vec2ui&, GetSize, (), (const, override));
    MOCK_METHOD(std::optional<Utils::Image>, GetImage, (IdType), (const, override));
    MOCK_METHOD(std::optional<Utils::Image>, GetImage, (FrameBuffer::Type), (const, override));
    MOCK_METHOD(void, UpdateDrawBuffers, (), (override));
};
}  // namespace GraphicsApi