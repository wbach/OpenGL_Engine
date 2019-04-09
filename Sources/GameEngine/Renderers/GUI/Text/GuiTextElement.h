#pragma once
#include <functional>
#include <string>
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "GraphicsApi/WindowApi.hpp"
#include "Types.h"
#include <optional>

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
    const GraphicsApi::Surface& GetSurface() const;
    std::optional<uint32> GetTextureId() const;
    const std::string& GetText() const;
    void SetTextureId(uint32 id);
    void UnsetTexture();

private:
    void RenderText();

private:
    UpdateTextureFunction updateTexture_;
    GraphicsApi::IWindowApi& windowApi_;
    std::string text_;
    uint32 outline_;
    uint32 fontSize_;
    std::optional<uint32> textureId_;
    uint32 fontId_;
    std::string font_;
    GraphicsApi::Surface surface_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
