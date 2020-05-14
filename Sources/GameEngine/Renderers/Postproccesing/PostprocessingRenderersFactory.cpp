#include "PostprocessingRenderersFactory.h"
#include "Blur/Blur.h"
#include "ColorFliper/ColorFliper.h"
#include "DefferedLighting/DefferedLighting.h"
#include "SSAO/SSAO.h"

namespace GameEngine
{
PostprocessingRenderersFactory::PostprocessingRenderersFactory(RendererContext& context)
    : context_(context)
{
}
std::unique_ptr<PostprocessingRenderer> PostprocessingRenderersFactory::Create(PostprocessingRendererType type)
{
    switch (type)
    {
        case PostprocessingRendererType::SSAO:
            return std::make_unique<SSAORenderer>(context_);
        case PostprocessingRendererType::BLUR:
            return std::make_unique<Blur>(context_);
        case PostprocessingRendererType::COLOR_FLIPER:
            return std::make_unique<ColorFliper>(context_);
        case PostprocessingRendererType::DEFFERED_LIGHT:
            return std::make_unique<DefferedLighting>(context_);
        case PostprocessingRendererType::CONTRAST_CHANGER:
            return nullptr;
        case PostprocessingRendererType::FXAA:
            return nullptr;
    }
    return nullptr;
}
}  // namespace GameEngine
