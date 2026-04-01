#pragma once
#include <functional>
#include <optional>
#include <string>

#include "GameEngine/Renderers/GUI/GuiRendererElementBase.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "IFontManager.h"
#include "Surface.h"
#include "Types.h"

namespace GameEngine
{
class GUIRenderer;
class IResourceManager;

class ENGINE_API GuiTextElement : public GuiRendererElementBase
{
public:
    enum class RenderMode
    {
        STRETCH,
        NATIVE
    };

    enum class Algin
    {
        LEFT,
        CENTER,
        RIGHT
    };

    struct FontInfo
    {
        uint32 outline_;
        uint32 fontSize_;
        File file_;
    };

public:
    GuiTextElement(IFontManager&, GUIRenderer&, IResourceManager&, const std::string& font);
    GuiTextElement(IFontManager&, GUIRenderer&, IResourceManager&, const std::string& font, const std::string& str);
    GuiTextElement(IFontManager&, GUIRenderer&, IResourceManager&, const std::string& font, const std::string& str, uint32 size);
    GuiTextElement(IFontManager&, GUIRenderer&, IResourceManager&, const std::string& font, const std::string& str, uint32 size,
                   uint32 outline);
    GuiTextElement(IFontManager&, GUIRenderer&, IResourceManager&, const std::string& font, const std::string& str, uint32 size,
                   uint32 outline, int wrapWidth);
    ~GuiTextElement();

public:
    std::optional<uint32> GetTextureId() const override;
    const std::string& GetText() const;
    void SetTexture(GeneralTexture*);
    void UnsetTexture();
    void setUniqueTextureName(const std::string&);
    void SetText(const std::string&);
    void Append(const std::string&);
    void Append(char);
    void Pop();
    void SetFontSize(uint32 size);
    void SetOutline(uint32 outline);
    void SetFont(const File& font);
    void SetAlgin(Algin algin);
    const FontInfo& GetFontInfo() const;
    mat4 GetTransformMatrix() const override;

    void SetLocalScale(const vec2& scale) override;
    void setParent(GuiElement*) override;
    void setRenderMode(RenderMode);
    RenderMode GetRenderMode() const;
    Algin GetAlgin() const;
    uint32 GetOutline() const;
    uint32 GetFontSize() const;
    const File& GetFontFile() const;

    void SetWrapWidth(uint32);
    uint32 GetWrapWith() const;

private:
    void UpdateTexture(IFontManager::TextureData);
    void RenderText(bool fontOverride = false);

private:
    RenderMode renderMode_{RenderMode::STRETCH};
    IFontManager& fontManager_;
    std::string text_;
    std::string textureName_;
    bool uniqueName_;
    FontInfo fontInfo_;
    std::optional<uint32> fontId_;
    bool openFontFailed_;
    Algin algin_;
    vec2 rendererdTextScale_;
    uint32 wrapWidth_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
