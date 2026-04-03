#include "GuiTextElement.h"

#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "Types.h"

namespace GameEngine
{
GuiTextElement::GuiTextElement(IFontManager& fontManager, GUIRenderer& guiRenderer, IResourceManager& resourceManager,
                               const std::string& inputText)
    : GuiRenderAble(resourceManager, guiRenderer, GuiElementTypes::Text)
    , fontManager_(fontManager)
    , uniqueName_{false}
    , openFontFailed_(false)
    , rendererdTextScale_(0)
{
    text.text = inputText;
}

GuiTextElement::~GuiTextElement()
{
    if (fontId_)
    {
        fontManager_.closeFont(*fontId_);
        fontId_.reset();
    }
}

const std::string& GuiTextElement::GetText() const
{
    return text.text;
}

void GuiTextElement::SetText(const std::string& input)
{
    if (input == text.text)
        return;

    text.text = input;
}

void GuiTextElement::Append(const std::string& input)
{
    text.text = text.text.get() + input;
}

void GuiTextElement::Append(char c)
{
    text.text = text.text.get() + c;
}

void GuiTextElement::Pop()
{
    if (not text.text->empty())
    {
        text.text.modify().pop_back();
    }
}

void GuiTextElement::UpdateTransformMatrix()
{
    auto scale          = GetScreenScale();
    auto factorX        = scale.x / rendererdTextScale_.x;
    auto factorY        = scale.y / rendererdTextScale_.y;

    auto finalFactor    = std::min(factorX, factorY);
    auto renderScale    = rendererdTextScale_ * finalFactor;
    auto renderPosition = GetScreenPosition();

    switch (*render.align)
    {
        case Align::LEFT:
            renderPosition.x += (renderScale.x / 2.f) - scale.x / 2.f;
            break;
        case Align::RIGHT:
            renderPosition.x -= renderScale.x / 2.f;
            renderPosition.x += scale.x / 2.f;
            break;
        default:
            break;
    }

    transformMatrix_ = Utils::CreateTransformationMatrix(renderPosition * 2.f - 1.f, renderScale, DegreesFloat(0.f));
}

void GuiTextElement::setUniqueTextureName(const std::string& name)
{
    textureName_ = name + "_" + std::to_string(GetId());
    uniqueName_  = true;

    if (texture_)
    {
        if (texture_->GetGraphicsObjectId())
        {
            resourceManager_.GetTextureLoader().UpdateTexture(texture_, textureName_);
        }
        return;
    }
}

vec2 ConvertSizeToScale(const vec2ui& size, const vec2ui& windowSize)
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

void GuiTextElement::UpdateTexture(IFontManager::TextureData data)
{
    if (texture_)
    {
        texture_->SetImage(std::move(data.image));

        if (texture_->GetGraphicsObjectId())
        {
            if (not uniqueName_)
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
    params.sizeLimitPolicy = SizeLimitPolicy::NoLimited;
    params.filter          = GraphicsApi::TextureFilter::LINEAR;
    texture_               = resourceManager_.GetTextureLoader().CreateTexture(textureName_, params, std::move(data.image));
}
void GuiTextElement::SyncWrapWidthWithParent()
{
    if (parent_ and render.mode == RenderMode::WRAPPED)
    {
        float parentWidth = parent_->GetScreenScale().x;
        uint32 pixels     = static_cast<uint32>(parentWidth * EngineConf.window.size->x / 2.0f);

        if (text.wrapWidth != pixels)
        {
            text.wrapWidth = pixels;
        }
    }
}
void GuiTextElement::UpdateTexture()
{
    auto isRenderTextNeeded = not texture_ or font.isDirty() or text.isDirty();
    if (not isRenderTextNeeded)
    {
        return;
    }

    if (text.text->empty())
    {
        DeleteTexture();
        return;
    }

    if (font.isDirty())
    {
        fontId_ = fontManager_.openFont(font.style, font.size, font.outline, font.file);
        font.clearDirty();
    }

    if (not fontId_)
    {
        DeleteTexture();
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
    rendererdTextScale_ = ConvertSizeToScale(imageData->image.size(), *EngineConf.window.size);

    if (not uniqueName_)
    {
        textureName_ = imageData->name;
    }

    if (render.mode == RenderMode::NATIVE)
    {
        transform_.scale = rendererdTextScale_;
        if (parent_)
            parent_->CallOnChange();
    }

    CallOnChange();
    UpdateTexture(std::move(*imageData));
}
}  // namespace GameEngine
