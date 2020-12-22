#pragma once
#include <functional>
#include <optional>
#include <string>
#include "GameEngine/Renderers/GUI/GuiRendererElementBase.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "Surface.h"
#include "Types.h"
#include "FontManager.h"

namespace GameEngine
{
class GUIRenderer;
class IResourceManager;

class GuiTextElement : public GuiRendererElementBase
{
public:
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
    GuiTextElement(FontManager&, GUIRenderer&, IResourceManager&, const std::string& font);
    GuiTextElement(FontManager&, GUIRenderer&, IResourceManager&, const std::string& font, const std::string& str);
    GuiTextElement(FontManager&, GUIRenderer&, IResourceManager&, const std::string& font, const std::string& str, uint32 size);
    GuiTextElement(FontManager&, GUIRenderer&, IResourceManager&, const std::string& font, const std::string& str, uint32 size, uint32 outline);

public:
    std::optional<uint32> GetTextureId() const;
    const std::string& GetText() const;
    void SetTexture(GeneralTexture*);
    void UnsetTexture();
    void SetText(const std::string&);
    void Append(const std::string&);
    void Append(char);
    void Pop();
    void SetFontSize(uint32 size);
    void SetOutline(uint32 outline);
    void SetFont(const File& font);
    void SetAlgin(Algin algin);
    void SetScale(const vec2&) override;
    void SetZPositionOffset(float offset) override;
    const FontInfo& GetFontInfo() const;

private:
    void UpdateTexture(FontManager::TextureData);
    void RenderText(bool fontOverride = false);
    void CalculateAlginOffset();

private:
    FontManager& fontManager_;
    std::string text_;
    std::string textureName_;
    FontInfo fontInfo_;
    std::optional<uint32> fontId_;
    bool openFontFailed_;
    Algin algin_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
