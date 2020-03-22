#pragma once
#include <stdexcept>
#include "Image.h"
#include "Texture.h"

namespace GameEngine
{
class HeightMap : public Texture
{
public:
    HeightMap(GraphicsApi::IGraphicsApi& graphicsApi, bool keepData, const std::string& file,
              const std::string& filepath, Image image);
    virtual void GpuLoadingPass() override;
    virtual void GpuPostLoadingPass() override;
    Image* GetImage();
    void SetScale(const vec3&);
    const vec3& GetScale() const;

private:
    Image image_;
    vec3 scale_;
    bool keepData_ = false;
};
}  // namespace GameEngine
