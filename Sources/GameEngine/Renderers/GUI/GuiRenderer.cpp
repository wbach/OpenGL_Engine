#include "GuiRenderer.h"

#include <algorithm>
#include <mutex>

#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "Logger/Log.h"
#include "Text/GuiTextElement.h"

namespace GameEngine
{
namespace
{
std::mutex subscriberMutex;

struct ColorBuffer
{
    AlignWrapper<vec4> color;
};
}  // namespace

GUIRenderer::GUIRenderer(GraphicsApi::IGraphicsApi& graphicsApi)
    : graphicsApi_(graphicsApi)
    , shader_(graphicsApi, GraphicsApi::ShaderProgramType::Texture)
    , isInit_{false}
{
}

GUIRenderer::~GUIRenderer()
{
    graphicsApi_.DeleteShaderBuffer(transformBuffer_);
    graphicsApi_.DeleteShaderBuffer(colorBuffer_);
}

void GUIRenderer::Init()
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
    LOG_DEBUG << "GUIRenderer is initialize status : " << isInit_;
}

void GUIRenderer::render()
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

    float min = 100000000.f;
    bool sortNeeded{false};

    for (const auto& subscriber : subscribers_)
    {
        if (not subscriber or not subscriber->IsShow() or not subscriber->IsActive() or not subscriber->GetTextureId())
            continue;

        PerObjectUpdate buffer;
        buffer.TransformationMatrix = graphicsApi_.PrepareMatrixToLoad(subscriber->GetTransformMatrix());
        graphicsApi_.UpdateShaderBuffer(transformBuffer_, &buffer);
        graphicsApi_.BindShaderBuffer(transformBuffer_);

        ColorBuffer colorBuffer;
        colorBuffer.color = subscriber->GetColor();
        graphicsApi_.UpdateShaderBuffer(colorBuffer_, &colorBuffer);
        graphicsApi_.BindShaderBuffer(colorBuffer_);

        graphicsApi_.ActiveTexture(0, *subscriber->GetTextureId());
        graphicsApi_.RenderQuad();

        if (subscriber->GetZValue() > min)
        {
            /* LOG TO FIX*/ LOG_ERROR << ("Sort needed");
            sortNeeded = true;
        }
        min = subscriber->GetZValue();
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

void GUIRenderer::SortSubscribers()
{
    std::sort(subscribers_.begin(), subscribers_.end(),
              [](const auto& l, const auto& r) { return l->GetZValue() > r->GetZValue(); });
}

void GUIRenderer::Subscribe(GuiElement& element)
{
    if (element.GetType() == GuiElementTypes::Text or element.GetType() == GuiElementTypes::Texture)
    {
        auto iter = std::find_if(subscribers_.begin(), subscribers_.end(),
                                 [&element](const auto& lelement) { return element.GetId() == lelement->GetId(); });

        if (iter != subscribers_.end())
        {
            /* LOG TO FIX*/ LOG_ERROR << ("Try duplicate subscribe element");
            return;
        }
        std::lock_guard<std::mutex> lk(subscriberMutex);
        auto guiBaseRendererElement = static_cast<GuiRendererElementBase*>(&element);
        subscribers_.push_back(guiBaseRendererElement);
        SortSubscribers();
    }
    else
    {
        /* LOG TO FIX*/ LOG_ERROR << ("Wrong type gui element, id: " + std::to_string(element.GetId()));
    }
}

void GUIRenderer::UnSubscribeAll()
{
    std::lock_guard<std::mutex> lk(subscriberMutex);
    subscribers_.clear();
}

void GUIRenderer::UnSubscribe(uint32 id)
{
    std::lock_guard<std::mutex> lk(subscriberMutex);
    auto iter = std::find_if(subscribers_.begin(), subscribers_.end(), [id](auto text) { return text->GetId() == id; });

    if (iter != subscribers_.end())
    {
        subscribers_.erase(iter);
    }
}

void GUIRenderer::UnSubscribe(const GuiElement& element)
{
    if (element.GetType() == GuiElementTypes::Text or element.GetType() == GuiElementTypes::Texture)
    {
        UnSubscribe(element.GetId());
    }
}

void GUIRenderer::ReloadShaders()
{
    shader_.Reload();
}
}  // namespace GameEngine
