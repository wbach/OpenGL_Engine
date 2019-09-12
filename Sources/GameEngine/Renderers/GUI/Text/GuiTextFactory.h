#pragma once
#include <functional>
#include <optional>
#include <string>
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "GraphicsApi/WindowApi.hpp"
#include "IGuiTextFactory.h"

#include "Types.h"

namespace GameEngine
{
class GuiElement;
class GuiTextElement;
class IResourceManager;

class GuiTextFactory : public IGuiTextFactory
{
public:
    GuiTextFactory(std::function<void(GuiElement&)> renderSubscribe,
                   std::function<void(const GuiElement&)> unsubscribeElement, IResourceManager&,
                   const vec2ui& windowSize);
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
    std::function<void(GuiElement&)> renderSubscribe_;
    std::function<void(const GuiElement&)> unsubscribeElement_;
};
}  // namespace GameEngine
