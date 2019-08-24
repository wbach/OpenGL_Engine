#include "GuiManager.h"
#include <algorithm>

namespace GameEngine
{
GuiManager::GuiManager(std::function<void(GuiElement&)> renderSubscribe,
                       std::function<void(const GuiElement&)> unsubscribeElement, std::function<void()> unsubscribeAll)
    : subscribe_(renderSubscribe)
    , unsubscribeAll_(unsubscribeAll)
    , unsubscribeElement_(unsubscribeElement)
{
}
void GuiManager::Add(std::unique_ptr<GuiElement> element)
{
    if (element == nullptr)
    {
        ERROR_LOG("Add nullptr");
        return;
    }

    subscribe_(*element.get());
    elements_.push_back(std::move(element));
}
GuiElement * GuiManager::GetElement(const std::string & label)
{
    auto element = std::find_if(elements_.begin(), elements_.end(), [&label](const auto& element) { return element->GetLabel() == label; });

    if (element != elements_.end())
    {
        return element->get();
    }
    else
    {
        DEBUG_LOG("Element with label " + label + " not found");
        return nullptr;
    }
}
GuiElement * GuiManager::GetElement(uint32 id)
{
    auto element = std::find_if(elements_.begin(), elements_.end(), [id](const auto& element) { return element->GetId() == id; });

    if (element != elements_.end())
    {
        return element->get();
    }
    else
    {
        return nullptr;
    }
}
const GuiElements& GuiManager::GetElements() const
{
    return elements_;
}

void GuiManager::Update()
{
    for (auto iter = elements_.begin(); iter != elements_.end();)
    {
        auto element = iter->get();

        if (element and element->IsMarkToRemove())
        {
            unsubscribeElement_(*element);
            iter = elements_.erase(iter);
        }
        else
        {
            element->Update();
            ++iter;
        }
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

bool GuiManager::SaveToFile(const std::string&)
{
    return false;
}

void GuiManager::Remove(uint32 id)
{
    auto element = GetElement(id);
    if (element)
    {
        Remove(*element);
    }
    else
    {
        ERROR_LOG("Element not found");
    }
}

void GuiManager::Remove(const GuiElement& element)
{
    auto id = element.GetId();

    auto iter =
        std::find_if(elements_.begin(), elements_.end(), [id](const auto& element) { return element->GetId() == id; });

    if (iter != elements_.end())
    {
        unsubscribeElement_(element);
        elements_.erase(iter);
    }
}

}  // namespace GameEngine
