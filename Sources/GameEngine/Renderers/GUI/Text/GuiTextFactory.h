#pragma once
#include <functional>
#include <optional>
#include <string>
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "GraphicsApi/WindowApi.hpp"

#include "Types.h"

namespace GameEngine
{
class GuiTextElement;
class IResourceManager;

class GuiTextFactory
{
public:
    GuiTextFactory(IResourceManager&, const vec2ui& windowSize);
    ~GuiTextFactory();

public:
    std::unique_ptr<GuiTextElement> Create(const std::string& font, const std::string& text, uint32 fontSize = 10,
                                           uint32 outline = 0);

private:
    void UpdateTexture(GuiTextElement&);

private:
    IResourceManager& resourceManager_;
    GraphicsApi::IWindowApi& windowApi_;
    const vec2ui windowSize_;
};
}  // namespace GameEngine
