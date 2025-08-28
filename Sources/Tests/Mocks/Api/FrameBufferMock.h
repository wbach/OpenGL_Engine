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
    //virtual bool Init()                                      = 0;
    //virtual void Bind(FrameBuffer::BindType)                 = 0;
    //virtual void UnBind()                                    = 0;
    //virtual void Clear()                                     = 0;
    //virtual void CleanUp()                                   = 0;
    //virtual ID GetAttachmentTexture(FrameBuffer::Type) const = 0;
    //virtual void TakeSnapshot(const std::string&)            = 0;
};
}  // namespace GraphicsApi