#pragma once
#include "FontParameters.h"
#include "GameEngine/Renderers/GUI/RenderAble.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "IFontManager.h"
#include "Property.h"
#include "RenderTextParameters.h"
#include "Surface.h"
#include "TextParameters.h"

namespace GameEngine
{
class IResourceManager;

namespace GUI
{
class ENGINE_API Text : public RenderAble
{
public:
public:
    Text(IFontManager&, IResourceManager&, Renderer&, const std::string&);
    ~Text() override;

public:
    const std::string& getText() const;
    void setText(const std::string&);
    void append(const std::string&);
    void append(char);
    void pop();

public:
    TextParameters text;
    FontParameters font;
    RenderTextParameters render;

private:
    void updateTransformMatrix() override;
    void updateTexture() override;
    void updateTexture(TextureData);
    void accept(IElementVisitor&) override;
    int calculatePtSize(const vec2& screenScale, const vec2ui& windowSize, float marginFactor = 0.8f);

private:
    IFontManager& fontManager_;

private:
    std::string textureName_;
    std::optional<IdType> fontId_;
    bool openFontFailed_;
    vec2 rendererdTextScale_;
};
}  // namespace GUI
}  // namespace GameEngine
