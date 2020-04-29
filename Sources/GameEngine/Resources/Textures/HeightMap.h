#pragma once
#include <stdexcept>
#include "Image.h"
#include "Texture.h"

namespace GameEngine
{
class HeightMap : public Texture
{
public:
    HeightMap(GraphicsApi::IGraphicsApi&, bool keepData, const std::string& file,
              const std::string& filepath, Image image);

    void GpuLoadingPass() override;

    Image& GetImage();
    const Image& GetImage() const;
    void SetScale(const vec3&);
    const vec3& GetScale() const;
    float GetMaximumHeight() const;

    bool SetHeight(const vec2ui&, float);

private:
    Image image_;
    vec3 scale_;
    bool keepData_ = false;
    float maximumHeight_;
};
}  // namespace GameEngine
