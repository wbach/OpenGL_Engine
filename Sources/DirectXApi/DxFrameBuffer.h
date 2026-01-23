#pragma once
#include <GraphicsApi/IFrameBuffer.h>

namespace DirectX
{
class DxFrameBuffer : public GraphicsApi::IFrameBuffer
{
public:
    bool Init() override;
    void Bind(GraphicsApi::FrameBuffer::BindType) override;
    void BindTexture(IdType, GraphicsApi::FrameBuffer::Type) override;
    void BindTextureLayer(IdType, GraphicsApi::FrameBuffer::Type, int) override;
    void UnBind() override;
    void Clear() override;
    void CleanUp() override;
    void UpdateDrawBuffers() override;
    GraphicsApi::ID GetAttachmentTexture(GraphicsApi::FrameBuffer::Type) const override;
    void TakeSnapshot(const std::filesystem::path&) override;
    const vec2ui& GetSize() const override;
    std::optional<Utils::Image> GetImage(IdType) const override;
    std::optional<Utils::Image> GetImage(GraphicsApi::FrameBuffer::Type) const override;
};
}  // namespace DirectX
