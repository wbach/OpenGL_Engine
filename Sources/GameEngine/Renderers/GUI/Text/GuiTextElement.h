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

private:
    void RenderText();

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

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
