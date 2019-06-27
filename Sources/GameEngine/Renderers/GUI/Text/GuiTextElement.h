#pragma once
#include <functional>
#include <optional>
#include <string>
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GraphicsApi/WindowApi.hpp"
#include "Types.h"

namespace GameEngine
{
class GuiTextElement;
typedef std::function<void(GuiTextElement&)> UpdateTextureFunction;

class GuiTextElement : public GuiElement
{
public:
    enum class Algin
    {
        LEFT,
        CENTER,
        RIGHT
    };

public:
    GuiTextElement(UpdateTextureFunction updateTexture, GraphicsApi::IWindowApi&, const vec2ui& windowSize,
                   const std::string& font);
    GuiTextElement(UpdateTextureFunction updateTexture, GraphicsApi::IWindowApi&, const vec2ui& windowSize,
                   const std::string& font, const std::string& str);
    GuiTextElement(UpdateTextureFunction updateTexture, GraphicsApi::IWindowApi&, const vec2ui& windowSize,
                   const std::string& font, const std::string& str, uint32 size);
    GuiTextElement(UpdateTextureFunction updateTexture, GraphicsApi::IWindowApi&, const vec2ui& windowSize,
                   const std::string& font, const std::string& str, uint32 size, uint32 outline);

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

private:
    void RenderText(bool fontOverride = false);
    void CalculateAlginOffset();

private:
    UpdateTextureFunction updateTexture_;
    GraphicsApi::IWindowApi& windowApi_;
    std::string text_;
    uint32 outline_;
    uint32 fontSize_;
    Texture* texture_;
    std::optional<uint32> fontId_;
    std::string font_;
    std::optional<GraphicsApi::Surface> surface_;
    bool openFontFailed_;
    Algin algin_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
