#pragma once
#include <memory>

#include "PostprocessingRendererTypes.h"

namespace GameEngine
{
class PostprocessingRenderer;

struct IPostprocessingRenderersFactory
{
    virtual ~IPostprocessingRenderersFactory() = default;

    virtual std::unique_ptr<PostprocessingRenderer> Create(PostprocessingRendererType type) = 0;
};
}  // namespace GameEngine
