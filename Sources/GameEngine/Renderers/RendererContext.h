#pragma once
#include <GraphicsApi/IGraphicsApi.h>
#include <Types.h>

#include <functional>
#include <memory>

#include "GameEngine/Engine/Configuration.h"
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
    RendererContext(Projection& projection, Frustrum& frustrum, GraphicsApi::IGraphicsApi& api, IGpuResourceLoader& gpuLoader,
                    Utils::MeasurementHandler& measurmentHandler, const Time& time)
        : projection_(projection)
        , frustrum_(frustrum)
        , graphicsApi_(api)
        , gpuLoader_(gpuLoader)
        , measurmentHandler_(measurmentHandler)
        , time_(time)
        , scene_(nullptr)
        , fogColor_(0.8f, 0.8f, 0.8f)
    {
    }
    Projection& projection_;
    Frustrum& frustrum_;
    GraphicsApi::IGraphicsApi& graphicsApi_;
    IGpuResourceLoader& gpuLoader_;
    Utils::MeasurementHandler& measurmentHandler_;
    const Time& time_;
    Scene* scene_;
    vec3 fogColor_;

    GraphicsApi::ID sharedTextures[magic_enum::enum_count<SharedTextures>()];
};
}  // namespace GameEngine
