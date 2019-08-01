#include "GuiManager.h"
#include <algorithm>
//#include <mutex>

namespace GameEngine
{
//namespace
//{
//std::mutex guiElementMutex;
//}
GuiManager::GuiManager(std::function<void(GuiElement&)> renderSubscribe,
                       std::function<void(GuiElement&)> unubscribeElement, std::function<void()> unsubscribeAll)
    : subscribe_(renderSubscribe)
    , unsubscribeAll_(unsubscribeAll)
    , unubscribeElement_(unubscribeElement)
{
}
void GuiManager::Add(const std::string& name, std::unique_ptr<GuiElement> element)
{
    if (element == nullptr)
    {
        DEBUG_LOG(name + " is nullptr");
        return;
    }

    if (elementsMap_.count(name) > 0)
    {
        DEBUG_LOG("REPLACE EXISTING Element name : " + name);
    }

    subscribe_(*element.get());
    elementsMap_.insert({name, element.get()});
    elements_.push_back(std::move(element));
}
const GuiElements& GuiManager::GetElements() const
{
    return elements_;
}

const GuiElementsMap& GuiManager::GetElementsMap() const
{
    return elementsMap_;
}

void GuiManager::Update()
{
    for (auto iter = elements_.begin(); iter != elements_.end();)
    {
        auto element = iter->get();

        if (element and element->IsMarkToRemove())
        {
            unubscribeElement_(*element);
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

void GuiManager::SaveToFile(const std::string&)
{
}

void GuiManager::Remove(const std::string& name)
{
    if (elementsMap_.count(name) == 0)
        return;

    auto id = elementsMap_.at(name)->GetId();

    auto iter =
        std::find_if(elements_.begin(), elements_.end(), [id](const auto& element) { return element->GetId() == id; });
    if (iter != elements_.end())
    {
        elements_.erase(iter);
        elementsMap_.erase(name);
    }
}

void GuiManager::Remove(const GuiElement& element)
{
    auto id = element.GetId();

    auto iter =
        std::find_if(elements_.begin(), elements_.end(), [id](const auto& element) { return element->GetId() == id; });
    auto mapiter = std::find_if(elementsMap_.begin(), elementsMap_.end(),
                                [id](const auto& pair) { return pair.second->GetId() == id; });

    if (iter != elements_.end() and mapiter != elementsMap_.end())
    {
        elements_.erase(iter);
        elementsMap_.erase(mapiter);
    }
}

}  // namespace GameEngine
