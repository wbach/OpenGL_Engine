#pragma once
#include <GraphicsApi/IGraphicsApi.h>
#include <Types.h>

#include "magic_enum/magic_enum.hpp"

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
class IGpuResourceLoader;
class ICamera;

enum class SharedTextures
{
    skyTexture,
    shadowCascade0,
    shadowCascade1,
    shadowCascade2,
    shadowCascade3
};

struct RendererContext
{
    RendererContext(Frustrum& frustrum, GraphicsApi::IGraphicsApi& api, IGpuResourceLoader& gpuLoader,
                    Utils::MeasurementHandler& measurmentHandler, const Time& time)
        : frustrum_(frustrum)
        , graphicsApi_(api)
        , gpuLoader_(gpuLoader)
        , measurmentHandler_(measurmentHandler)
        , time_(time)
        , scene_(nullptr)
        , camera_(nullptr)
        , fogColor_(0.8f, 0.8f, 0.8f)
    {
    }
    Frustrum& frustrum_;
    GraphicsApi::IGraphicsApi& graphicsApi_;
    IGpuResourceLoader& gpuLoader_;
    Utils::MeasurementHandler& measurmentHandler_;
    const Time& time_;
    Scene* scene_;
    ICamera* camera_;
    vec3 fogColor_;

    GraphicsApi::ID sharedTextures[magic_enum::enum_count<SharedTextures>()];
};
}  // namespace GameEngine
