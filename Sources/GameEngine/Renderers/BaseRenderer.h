#pragma once
#include <Utils/MeasurementHandler.h>
#include <Utils/Utils.h>

#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "GameEngine/Components/IComponent.h"
#include "GameEngine/Engine/Engine.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "IRenderer.h"
#include "Postproccesing/PostprocessingRenderersManager.h"
#include "RendererContext.h"
#include "Types.h"

namespace GameEngine
{
class Projection;
using RendererPtr = std::unique_ptr<IRenderer>;

struct RendererInfo
{
    MeasurementValue* renderTime{nullptr};
    MeasurementValue* prepareTime{nullptr};
    RendererPtr ptr;
};

using Renderers = std::vector<RendererInfo>;

class BaseRenderer : public IRenderer
{
public:
    BaseRenderer(RendererContext&);
    ~BaseRenderer();

    void init() override;
    void prepare() override;
    void render() override;
    virtual void setViewPort();
    void blendRender() override;
    void subscribe(GameObject&) override;
    void unSubscribe(GameObject&) override;
    void unSubscribe(const Components::IComponent&) override;
    void unSubscribeAll() override;
    void reloadShaders() override;

protected:
    void initRenderers();
    void createRenderers();

    template <class T>
    void addRenderer()
    {
        RendererInfo info{};

        if (EngineConf.debugParams.showRenderersTimers)
        {
            info.renderTime  = &context_.measurmentHandler_.AddNewMeasurment(Utils::GetTypeName<T>() + " render time : ", "0");
            info.prepareTime = &context_.measurmentHandler_.AddNewMeasurment(Utils::GetTypeName<T>() + " prepare time : ", "0");
        }

        info.ptr = std::make_unique<T>(context_);
        renderers.push_back(std::move(info));
    }

protected:
    RendererContext& context_;
    Renderers renderers;
};
}  // namespace GameEngine
