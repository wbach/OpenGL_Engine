#pragma once
#include <Utils/MeasurementHandler.h>
#include <Utils/Utils.h>

#include <memory>
#include <vector>

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Components/IComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GraphicsApi/IFrameBuffer.h"
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
    void setRenderTarget(GraphicsApi::IFrameBuffer*);

protected:
    void initRenderers();
    void createRenderers();
    void renderImpl();
    void bindTarget();

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
    GraphicsApi::IFrameBuffer* renderTarget;
    Renderers renderers;
};
}  // namespace GameEngine
