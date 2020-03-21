#include "GuiRenderer.h"
#include <algorithm>
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GameEngine/Shaders/IShaderProgram.h"

#include "Text/GuiTextElement.h"

namespace GameEngine
{
namespace
{
std::mutex subscriberMutex;

struct ColorBuffer
{
    AlignWrapper<vec3> color;
};
}  // namespace

GUIRenderer::GUIRenderer(GraphicsApi::IGraphicsApi& graphicsApi, IShaderFactory& shaderFactory)
    : graphicsApi_(graphicsApi)
    , shaderFactory_(shaderFactory)
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
    auto id = graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate));
    if (not id)
    {
        isInit_ = false;
        return;
    }
    transformBuffer_ = *id;

    id = graphicsApi_.CreateShaderBuffer(PER_MESH_OBJECT_BIND_LOCATION, sizeof(ColorBuffer));
    if (not id)
    {
        isInit_ = false;
        return;
    }
    colorBuffer_ = *id;

    shader_ = shaderFactory_.create(GraphicsApi::Shaders::Texture);
    isInit_ = shader_->Init();

    DEBUG_LOG("GUIRenderer is initialize status : " + std::to_string(isInit_));
}

void GUIRenderer::Render(const Scene&, const Time&)
{
    if (not isInit_ or subscribers_.empty())
    {
        return;
    }

    shader_->Start();
    graphicsApi_.EnableBlend();
    graphicsApi_.DisableDepthTest();
    graphicsApi_.DisableDepthMask();
    graphicsApi_.SetBlendFunction(GraphicsApi::BlendFunctionType::ALPHA_ONE_MINUS_ALPHA);
    graphicsApi_.ActiveTexture(0);

    std::lock_guard<std::mutex> lk(subscriberMutex);

    float min = 100000000.f;
    bool sortNeeded{false};

    for (const auto& subscriber : subscribers_)
    {
        if (not subscriber or not subscriber->IsShow() or not subscriber->IsActive() or not subscriber->GetTextureId())
            continue;

        PerObjectUpdate buffer;
        buffer.TransformationMatrix = subscriber->GetTransformMatrix();
        graphicsApi_.UpdateShaderBuffer(transformBuffer_, &buffer);
        graphicsApi_.BindShaderBuffer(transformBuffer_);

        ColorBuffer colorBuffer;
        colorBuffer.color = subscriber->GetColor();
        graphicsApi_.UpdateShaderBuffer(colorBuffer_, &colorBuffer);
        graphicsApi_.BindShaderBuffer(colorBuffer_);

        graphicsApi_.BindTexture(*subscriber->GetTextureId());
        graphicsApi_.RenderQuad();

        if (subscriber->GetZValue() > min)
        {
            DEBUG_LOG("Sort needed");
            sortNeeded = true;
        }
        min = subscriber->GetZValue();
    }

    if (sortNeeded)
    {
        SortSubscribers();
    }

    shader_->Stop();
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
            ERROR_LOG("Try duplicate subscribe element");
            return;
        }
        std::lock_guard<std::mutex> lk(subscriberMutex);
        auto guiBaseRendererElement = static_cast<GuiRendererElementBase*>(&element);
        subscribers_.push_back(guiBaseRendererElement);
        SortSubscribers();
    }
    else
    {
        ERROR_LOG("Wrong type gui element, id: " + std::to_string(element.GetId()));
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
    shader_->Stop();
    shader_->Reload();
    shader_->Init();
}
}  // namespace GameEngine
