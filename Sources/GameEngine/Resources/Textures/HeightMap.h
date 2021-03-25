#pragma once
#include <stdexcept>
#include <GraphicsApi/Image.h>
#include "Texture.h"

namespace GameEngine
{
class HeightMap : public Texture
{
public:
    HeightMap(GraphicsApi::IGraphicsApi&, const TextureParameters&, const File&, GraphicsApi::Image);

    void GpuLoadingPass() override;

    //GraphicsApi::Image& GetImage();
    void setImage(GraphicsApi::Image);
    const GraphicsApi::Image& GetImage() const;
    float GetMaximumHeight() const;
    float GetMinimumHeight() const;
    float GetDeltaHeight() const;

    bool SetHeight(const vec2ui&, float);
    std::optional<float> GetHeight(const vec2ui&);
    bool UpdateMaximumHeight();

private:
    GraphicsApi::Image image_;
    float maximumHeight_;
    float minimumHeight_;
    float deltaHeight_;
};
}  // namespace GameEngine
