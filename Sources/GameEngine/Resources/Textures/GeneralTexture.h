#pragma once
#include <stdexcept>

#include "Image.h"
#include "Texture.h"

namespace GameEngine
{
class GeneralTexture : public Texture
{
public:
    GeneralTexture(GraphicsApi::IGraphicsApi&, const vec2ui&, void*);
    void GpuLoadingPass() override;

private:
    void* data_;
};
}  // namespace GameEngine
