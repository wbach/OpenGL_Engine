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

void GuiManager::Update()
{
    for (auto& element : elements_)
    {
        element->Update();
    }
}

ActionFunction GuiManager::GetActionFunction(const std::string& name)
{
    if (registeredActions_.count(name))
    {
        return registeredActions_.at(name);
    }
    return []() { DEBUG_LOG("Button action not found."); };
}

void GuiManager::RegisterAction(const std::string& name, ActionFunction action)
{
    registeredActions_.insert({name, action});
}

}  // namespace GameEngine
