#pragma once
#include <stdexcept>
#include "Image.h"
#include "Texture.h"

namespace GameEngine
{
class GeneralTexture : public Texture
{
public:
    GeneralTexture(GraphicsApi::IGraphicsApi& graphicsApi, const std::string& file,
                   const vec2ui& size, void* data);
    void GpuLoadingPass() override;

private:
    void* data_;
    vec2ui size_;
};
}  // namespace GameEngine
