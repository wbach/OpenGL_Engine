#include "GuiManager.h"
#include <Utils/XML/XmlWriter.h>
#include <algorithm>

namespace GameEngine
{
namespace
{
const std::string DEFAULT_ACTION = "DefaultAction";
}
void GuiManager::Add(std::unique_ptr<GuiElement> element)
{
    elements_.push_back(std::move(element));
}
GuiElement* GuiManager::GetElement(const std::string& label)
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
GuiElement* GuiManager::GetElement(uint32 id)
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
            iter = elements_.erase(iter);
        }
        else
        {
            element->Update();
            ++iter;
        }
    }

    if (not tasks_.empty())
    {
        for (auto& task : tasks_)
        {
            task();
        }
        tasks_.clear();
    }
}

ActionFunction GuiManager::GetActionFunction(const std::string& name)
{
    if (registeredActions_.count(name))
    {
        return registeredActions_.at(name);
    }
    if (registeredActions_.count(DEFAULT_ACTION))
    {
        return registeredActions_.at(DEFAULT_ACTION);
    }
    return [](auto&) { DEBUG_LOG("Button action not found. Default action not set."); };
}

void GuiManager::AddTask(std::function<void()> task)
{
    tasks_.push_back(task);
}

void GuiManager::RegisterAction(const std::string& name, ActionFunction action)
{
    registeredActions_.insert({name, action});
}

void GuiManager::RegisterDefaultAction(ActionFunction action)
{
    registeredActions_.insert({DEFAULT_ACTION, action});
}

bool GuiManager::SaveToFile(const std::string& filename)
{
    Utils::XmlNode root("gui");
    Utils::Xml::Write(filename, root);
    return true;
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

    auto iter = std::find_if(elements_.begin(), elements_.end(), [id](const auto& element) { return element->GetId() == id; });

    if (iter != elements_.end())
    {
        elements_.erase(iter);
    }
}

void GuiManager::RemoveNotPermaments()
{
    DEBUG_LOG("");
    for (auto iter = elements_.begin(); iter != elements_.end();)
    {
        if (not(*iter)->IsPermament())
        {
            DEBUG_LOG("Delete : " + std::to_string(iter->get()->GetId()));
            iter = elements_.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void GuiManager::RemoveAll()
{
    elements_.clear();
}

}  // namespace GameEngine
