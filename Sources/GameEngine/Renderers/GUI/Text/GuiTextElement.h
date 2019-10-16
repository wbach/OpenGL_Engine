#pragma once
#include <functional>
#include <optional>
#include <string>
#include "GameEngine/Renderers/GUI/GuiRendererElementBase.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GraphicsApi/WindowApi.hpp"
#include "Types.h"

namespace GameEngine
{
class GuiTextElement;
typedef std::function<void(GuiTextElement&)> UpdateTextureFunction;

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
        std::string font_;
    };

public:
    GuiTextElement(std::function<void(GuiElement&)> renderSubscribe, std::function<void(const GuiElement&)> unsubscribeElement, UpdateTextureFunction updateTexture, GraphicsApi::IWindowApi&, const vec2ui& windowSize, const std::string& font);
    GuiTextElement(std::function<void(GuiElement&)> renderSubscribe, std::function<void(const GuiElement&)> unsubscribeElement, UpdateTextureFunction updateTexture, GraphicsApi::IWindowApi&, const vec2ui& windowSize, const std::string& font, const std::string& str);
    GuiTextElement(std::function<void(GuiElement&)> renderSubscribe, std::function<void(const GuiElement&)> unsubscribeElement, UpdateTextureFunction updateTexture, GraphicsApi::IWindowApi&, const vec2ui& windowSize, const std::string& font, const std::string& str, uint32 size);
    GuiTextElement(std::function<void(GuiElement&)> renderSubscribe, std::function<void(const GuiElement&)> unsubscribeElement, UpdateTextureFunction updateTexture, GraphicsApi::IWindowApi&, const vec2ui& windowSize, const std::string& font, const std::string& str, uint32 size, uint32 outline);

public:
    const std::optional<GraphicsApi::Surface>& GetSurface() const;
    std::optional<uint32> GetTextureId() const;
    const std::string& GetText() const;
    void SetTexture(Texture*);
    void UnsetTexture();
    void SetText(const std::string&);
    void Append(const std::string&);
    void Append(char);
    void Pop();
    void SetFontSize(uint32 size);
    void SetOutline(uint32 outline);
    void SetFont(const std::string& font);
    void SetAlgin(Algin algin);
    void SetScale(const vec2&) override;
    void SetZPositionOffset(float offset) override;
    const FontInfo& GetFontInfo() const;

private:
    void RenderText(bool fontOverride = false);
    void CalculateAlginOffset();

private:
    UpdateTextureFunction updateTexture_;
    GraphicsApi::IWindowApi& windowApi_;
    std::string text_;
    FontInfo fontInfo_;
    std::optional<uint32> fontId_;
    std::optional<GraphicsApi::Surface> surface_;
    bool openFontFailed_;
    Algin algin_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
