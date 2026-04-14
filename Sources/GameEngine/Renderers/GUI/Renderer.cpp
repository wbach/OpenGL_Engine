#include "Renderer.h"

#include <Logger/Log.h>

#include <algorithm>
#include <limits>
#include <mutex>

#include "GameEngine/Renderers/GUI/RenderAble.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "Text/Text.h"

namespace GameEngine
{
namespace GUI
{

namespace
{
std::mutex subscriberMutex;

struct ColorBuffer
{
    AlignWrapper<vec4> color;
};
}  // namespace

Renderer::Renderer(GraphicsApi::IGraphicsApi& graphicsApi)
    : graphicsApi_(graphicsApi)
    , shader_(graphicsApi, GraphicsApi::ShaderProgramType::Texture)
    , isInit_{false}
{
}

Renderer::~Renderer()
{
    graphicsApi_.DeleteShaderBuffer(transformBuffer_);
    graphicsApi_.DeleteShaderBuffer(colorBuffer_);
    shader_.Clear();
}

void Renderer::Init()
{
    auto id =
        graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate), GraphicsApi::DrawFlag::Dynamic);
    if (not id)
    {
        isInit_ = false;
        return;
    }
    transformBuffer_ = *id;

    id = graphicsApi_.CreateShaderBuffer(PER_MESH_OBJECT_BIND_LOCATION, sizeof(ColorBuffer), GraphicsApi::DrawFlag::Dynamic);
    if (not id)
    {
        isInit_ = false;
        return;
    }
    colorBuffer_ = *id;

    shader_.Init();

    isInit_ = true;
    LOG_DEBUG << "Renderer is initialize status : " << isInit_;
}

void Renderer::render()
{
    if (not isInit_ or not shader_.IsReady() or subscribers_.empty())
    {
        return;
    }

    shader_.Start();
    graphicsApi_.EnableBlend();
    graphicsApi_.DisableDepthTest();
    graphicsApi_.DisableDepthMask();
    graphicsApi_.SetBlendFunction(GraphicsApi::BlendFunctionType::ONE_MINUS_SRC_ALPHA);
    std::lock_guard<std::mutex> lk(subscriberMutex);

    auto min = std::numeric_limits<float>::max();
    bool sortNeeded{false};

    for (auto& subscriber : subscribers_)
    {
        if (not subscriber or not subscriber->isActive())
        {
            subscriber->onSkipRender();
            continue;
        }

        subscriber->onRender();

        if (not subscriber->getTextureId())
            continue;

        PerObjectUpdate buffer;
        buffer.TransformationMatrix = graphicsApi_.PrepareMatrixToLoad(subscriber->getTransformMatrix());
        graphicsApi_.UpdateShaderBuffer(transformBuffer_, &buffer);
        graphicsApi_.BindShaderBuffer(transformBuffer_);

        ColorBuffer colorBuffer;
        colorBuffer.color = subscriber->getColor().value;
        graphicsApi_.UpdateShaderBuffer(colorBuffer_, &colorBuffer);
        graphicsApi_.BindShaderBuffer(colorBuffer_);

        graphicsApi_.ActiveTexture(0, *subscriber->getTextureId());
        graphicsApi_.RenderQuad();

        if (subscriber->getZValue() > min)
        {
            LOG_DEBUG << "Sort needed";
            sortNeeded = true;
        }
        min = subscriber->getZValue();
    }

    if (sortNeeded)
    {
        SortSubscribers();
    }

    shader_.Stop();
    graphicsApi_.DisableBlend();
    graphicsApi_.EnableDepthMask();
    graphicsApi_.EnableDepthTest();
}

void Renderer::SortSubscribers()
{
    std::sort(subscribers_.begin(), subscribers_.end(),
              [](const auto& l, const auto& r) { return l->getZValue() > r->getZValue(); });
}

void Renderer::Subscribe(Element& element)
{
    if (auto renderAbleElement = dynamic_cast<RenderAble*>(&element))
    {
        auto iter = std::find_if(subscribers_.begin(), subscribers_.end(),
                                 [&element](const auto& lelement) { return element.getId() == lelement->getId(); });

        if (iter != subscribers_.end())
        {
            LOG_WARN << "Try duplicate subscribe element";
            return;
        }
        std::lock_guard<std::mutex> lk(subscriberMutex);
        subscribers_.push_back(renderAbleElement);
        SortSubscribers();
    }
    else
    {
        LOG_ERROR << "Wrong type gui element, id: " << element.getId();
    }
}

void Renderer::UnSubscribeAll()
{
    std::lock_guard<std::mutex> lk(subscriberMutex);
    subscribers_.clear();
}

void Renderer::UnSubscribe(uint32 id)
{
    std::lock_guard<std::mutex> lk(subscriberMutex);
    auto iter = std::find_if(subscribers_.begin(), subscribers_.end(), [id](auto text) { return text->getId() == id; });

    if (iter != subscribers_.end())
    {
        subscribers_.erase(iter);
    }
}

void Renderer::UnSubscribe(const Element& element)
{
    UnSubscribe(element.getId());
}

void Renderer::ReloadShaders()
{
    shader_.Reload();
}
}  // namespace GUI
}  // namespace GameEngine
