#pragma once
#include "IPostprocessingRenderersFactory.h"

namespace GameEngine
{
struct RendererContext;

class PostprocessingRenderersFactory : public IPostprocessingRenderersFactory
{
public:
    PostprocessingRenderersFactory(RendererContext& context);
    std::unique_ptr<PostprocessingRenderer> Create(PostprocessingRendererType) override;

private:
    RendererContext& context_;
};
}  // namespace GameEngine
