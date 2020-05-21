#pragma once
#include <stdexcept>
#include "Image.h"
#include "Texture.h"

namespace GameEngine
{
class HeightMap : public Texture
{
public:
    HeightMap(GraphicsApi::IGraphicsApi&, const File&, Image);

    void GpuLoadingPass() override;

    Image& GetImage();
    const Image& GetImage() const;
    void SetScale(const vec3&);
    const vec3& GetScale() const;
    float GetMaximumHeight() const;
    float GetMinimumHeight() const;
    float GetDeltaHeight() const;

    bool SetHeight(const vec2ui&, float);
    std::optional<float> GetHeight(const vec2ui&);
    bool UpdateMaximumHeight();

private:
    Image image_;
    vec3 scale_;
    float maximumHeight_;
    float minimumHeight_;
    float deltaHeight_;
};
}  // namespace GameEngine
