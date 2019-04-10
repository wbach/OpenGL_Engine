#include "GuiManager.h"

namespace GameEngine
{
GuiManager::GuiManager(std::function<void(GuiElement&)> renderSubscribe)
    : subscribe_(renderSubscribe)
{
}
void GuiManager::Add(const std::string& name, std::unique_ptr<GuiElement> element)
{
    elementsMap_.insert({name, element.get()});
    elements_.push_back(std::move(element));
    subscribe_(*elements_.back());
}
const GuiElements& GuiManager::GetElements() const
{
    return elements_;
}
}  // namespace GameEngine
