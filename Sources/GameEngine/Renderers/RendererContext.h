#pragma once
#include <GraphicsApi/IGraphicsApi.h>
#include <Types.h>

#include <functional>
#include <memory>

namespace Utils
{
class MeasurementHandler;
}  // namespace Utils

namespace GameEngine
{
struct Time;
class Scene;
class Projection;
class PostprocessFrameBuffer;
class Frustrum;
class GeneralTexture;

struct RendererContext
{
    RendererContext(Projection& projection, Frustrum& frustrum, GraphicsApi::IGraphicsApi& api,
                    Utils::MeasurementHandler& measurmentHandler, const Time& time)
        : projection_(projection)
        , frustrum_(frustrum)
        , graphicsApi_(api)
        , measurmentHandler_(measurmentHandler)
        , time_(time)
        , scene_(nullptr)
    {
    }
    Projection& projection_;
    Frustrum& frustrum_;
    GraphicsApi::IGraphicsApi& graphicsApi_;
    Utils::MeasurementHandler& measurmentHandler_;
    const Time& time_;
    mat4 toShadowMapZeroMatrix_ = glm::mat4(1.f);
    GraphicsApi::ID shadowMapId_;
    Scene* scene_;
};
}  // namespace GameEngine
