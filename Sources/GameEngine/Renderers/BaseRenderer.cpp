//#include "BaseRenderer.h"
//
//namespace GameEngine
//{
//Renderer::Renderer(IGraphicsApi& graphicsApi, Projection& projection, IShaderFactory& shaderFactory,
//                   std::function<void(RendererFunctionType, RendererFunction)> rendererFunction)
//    : context_(&projection, graphicsApi, std::make_shared<DefferedFrameBuffer>(graphicsApi),
//        std::make_shared<ShadowFrameBuffer>(graphicsApi), shaderFactory, rendererFunction)
//    , postprocessingRenderersManager_(context_)
//{
//}
//}  // namespace GameEngine
