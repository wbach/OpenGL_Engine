#pragma once
#include <Types.h>

#include <functional>
#include <optional>
#include <string>

#include "GameEngine/Renderers/GUI/GuiRenderAble.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "IFontManager.h"
#include "Surface.h"

namespace GameEngine
{
class GUIRenderer;
class IResourceManager;

enum class RenderMode
{
    FIT,
    NATIVE,
    WRAPPED
};

enum class Align
{
    LEFT,
    CENTER,
    RIGHT
};

class GuiRenderTextParameters : public Params
{
public:
    Property<RenderMode> mode{RenderMode::FIT, &dirty};
    Property<Align> align{Align::CENTER, &dirty};
};

class GuiFontParameters : public Params
{
public:
    Property<FontStyle> style{FontStyle::Normal, &dirty};
    Property<uint32> outline{0, &dirty};
    Property<uint32> size{24, &dirty};
    Property<std::optional<File>> file{{}, &dirty};
};

class GuiTextParameters : public Params
{
public:
    Property<std::optional<uint32>> wrapWidth{std::nullopt, &dirty};
    Property<std::string> text{"", &dirty};
};

class ENGINE_API GuiTextElement : public GuiRenderAble
{
public:
public:
    GuiTextElement(IFontManager&, GUIRenderer&, IResourceManager&, const std::string&);
    ~GuiTextElement() override;

public:
    const std::string& GetText() const;

    void setUniqueTextureName(const std::string&);

    void SetText(const std::string&);
    void Append(const std::string&);
    void Append(char);
    void Pop();

public:
    GuiTextParameters text;
    GuiFontParameters font;
    GuiRenderTextParameters render;

private:
    void SyncWrapWidthWithParent();
    void UpdateTransformMatrix() override;
    void UpdateTexture() override;
    void UpdateTexture(IFontManager::TextureData);

private:
    IFontManager& fontManager_;

private:
    std::string textureName_;
    bool uniqueName_;
    std::optional<IdType> fontId_;
    bool openFontFailed_;
    vec2 rendererdTextScale_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
