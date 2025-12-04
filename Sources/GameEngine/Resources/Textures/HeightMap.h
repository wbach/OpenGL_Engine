#pragma once
#include <stdexcept>
#include <Utils/Image/Image.h>
#include "Texture.h"
#include <mutex>

namespace GameEngine
{
class HeightMap : public Texture
{
public:
    HeightMap(GraphicsApi::IGraphicsApi&, const TextureParameters&, const File&, Utils::Image&&);

    void GpuLoadingPass() override;

    //GraphicsApi::Image& GetImage();
    void setImage(Utils::Image&&);
    void setImageData(Utils::ImageData&&);
    const Utils::Image& GetImage() const;
    float GetMaximumHeight() const;
    float GetMinimumHeight() const;
    float GetDeltaHeight() const;

    bool SetHeight(const vec2ui&, float);
    std::optional<float> GetHeight(const vec2ui&);
    bool UpdateMaximumHeight();
    void SetScale(const vec3&);
    const vec3& GetScale() const;

private:
    mutable std::mutex mutex;
    Utils::Image image_;
    float maximumHeight_;
    float minimumHeight_;
    float deltaHeight_;
    vec3 scale_;
};
}  // namespace GameEngine
