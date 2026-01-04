#pragma once
#include <GraphicsApi/IFrameBuffer.h>

namespace DirectX
{
class DxFrameBuffer : public GraphicsApi::IFrameBuffer
{
public:
    bool Init() override;
    void Bind(GraphicsApi::FrameBuffer::BindType) override;
    void UnBind() override;
    void Clear() override;
    void CleanUp() override;
    GraphicsApi::ID GetAttachmentTexture(GraphicsApi::FrameBuffer::Type) const override;
    void TakeSnapshot(const std::string&) override;
    const vec2ui& GetSize() const override;
    std::optional<Utils::Image> GetImage(IdType) const override;
    std::optional<Utils::Image> GetImage(GraphicsApi::FrameBuffer::Type) const override;
};
}  // namespace DirectX
