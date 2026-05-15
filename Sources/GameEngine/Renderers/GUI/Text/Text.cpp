#include "Text.h"

#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "Types.h"

namespace GameEngine
{
namespace GUI
{
Text::Text(IFontManager& fontManager, IResourceManager& resourceManager, GUI::Renderer& renderer, const std::string& inputText)
    : RenderAble(resourceManager, renderer)
    , fontManager_(fontManager)
    , openFontFailed_(false)
    , rendererdTextScale_(0)
{
    text.text = inputText;
    subId_    = EngineConf.renderer.resolution.subscribeForChange(
        [this]()
        {
            LOG_DEBUG << "Resolution change";
            text.markAsDirty();
            invalidate();
        });
}

Text::Text(const Text& other)
    : RenderAble(other)
    , text(other.text)
    , font(other.font)
    , render(other.render)
    , fontManager_(other.fontManager_)
    , openFontFailed_(false)
    , rendererdTextScale_(0)
{
    subId_ = EngineConf.renderer.resolution.subscribeForChange(
        [this]()
        {
            LOG_DEBUG << "Resolution change";
            text.markAsDirty();
            invalidate();
        });
}

Text::~Text()
{
    if (fontId_)
    {
        fontManager_.closeFont(*fontId_);
        fontId_.reset();
    }

    EngineConf.renderer.resolution.unsubscribe(subId_);
}

const std::string& Text::getText() const
{
    return text.text;
}

void Text::setText(const std::string& input)
{
    if (input == text.text)
        return;

    text.text = input;
}

void Text::append(const std::string& input)
{
    text.text = text.text.get() + input;
}

void Text::append(char c)
{
    text.text = text.text.get() + c;
}

void Text::pop()
{
    if (not text.text->empty())
    {
        text.text.modify().pop_back();
    }
}

void Text::updateTransformMatrix()
{
    auto scale   = getScreenScale();
    auto factorX = scale.x / rendererdTextScale_.x;
    auto factorY = scale.y / rendererdTextScale_.y;

    auto finalFactor    = std::min(factorX, factorY);
    auto renderScale    = rendererdTextScale_ * finalFactor;
    auto renderPosition = getScreenPosition();

    switch (*render.align)
    {
        case HorizontalAlign::LEFT:
            renderPosition.x += (renderScale.x / 2.f) - scale.x / 2.f;
            break;
        case HorizontalAlign::RIGHT:
            renderPosition.x -= renderScale.x / 2.f;
            renderPosition.x += scale.x / 2.f;
            break;
        default:
            break;
    }

    transformMatrix_ = Utils::CreateTransformationMatrix(renderPosition * 2.f - 1.f, renderScale, DegreesFloat(0.f));
}

vec2 convertSizeToScale(const vec2ui& size, const vec2ui& windowSize)
{
    return vec2{static_cast<float>(size.x) / static_cast<float>(windowSize.x),
                static_cast<float>(size.y) / static_cast<float>(windowSize.y)} *
           2.f;
}

vec2ui ConvertScaleToSize(const vec2& scale, const vec2ui& windowSize)
{
    vec2ui size(0);
    size.x = static_cast<uint32>(scale.x * static_cast<float>(windowSize.x));
    size.y = static_cast<uint32>(scale.y * static_cast<float>(windowSize.y));
    return size;
}

void Text::updateTexture(TextureData data)
{
    if (texture_)
    {
        texture_->SetImage(std::move(data.image));

        if (texture_->GetGraphicsObjectId())
        {
            if (label.empty())
            {
                resourceManager_.GetTextureLoader().UpdateTexture(texture_, data.name);
            }
            else
            {
                resourceManager_.GetTextureLoader().UpdateTexture(*texture_);
            }
        }

        return;
    }

    TextureParameters params;
    params.sizeLimit = std::nullopt;;
    params.filter          = GraphicsApi::TextureFilter::LINEAR;
    texture_               = resourceManager_.GetTextureLoader().CreateTexture(textureName_, params, std::move(data.image));
}
void Text::updateTexture()
{
    auto isRenderTextNeeded = not texture_ or font.isDirty() or text.isDirty();
    if (not isRenderTextNeeded)
    {
        return;
    }

    if (text.text->empty())
    {
        deleteTexture();
        return;
    }

    if (not fontId_ or font.isDirty())
    {
        auto fontSize = font.size->value_or(calculatePtSize(getScreenScale(), *EngineConf.window.size, 2.f));
        fontId_       = fontManager_.openFont(font.style, fontSize, font.outline, font.file);
        font.clearDirty();
    }

    if (not fontId_)
    {
        deleteTexture();
        return;
    }

    if (not isRenderTextNeeded)
    {
        return;
    }

    auto imageData = fontManager_.renderText(text.text, *fontId_, text.wrapWidth);
    if (not imageData)
    {
        LOG_ERROR << "RenderFont error for : " << text.text;
        return;
    }

    text.clearDirty();
    rendererdTextScale_ = convertSizeToScale(imageData->image.size(), *EngineConf.window.size);
    textureName_        = label.empty() ? imageData->name : label;

    if (render.mode == RenderMode::NATIVE)
    {
        transform.scale = rendererdTextScale_;
        // if (parent)
        //     parent->onChange();
    }

    // invalidate();
    updateTexture(std::move(*imageData));
}
void Text::accept(IElementVisitor& visitor) const
{
    visitor.visit(*this);
}
int Text::calculatePtSize(const vec2& screenScale, const vec2ui& windowSize, float marginFactor)
{
    auto targetHeightPixels = (screenScale.y / 2.0f) * static_cast<float>(windowSize.y);
    auto ptSize             = static_cast<int>(targetHeightPixels * marginFactor);
    return std::clamp(ptSize, 4, 144);
}
std::unique_ptr<Element> Text::clone() const
{
    return std::make_unique<Text>(*this);
}

}  // namespace GUI
}  // namespace GameEngine
