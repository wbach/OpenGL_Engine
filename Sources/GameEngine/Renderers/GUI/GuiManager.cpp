#include "GuiManager.h"
#include <Utils/XML/XmlWriter.h>
#include <algorithm>

namespace GameEngine
{
namespace
{
const std::string DEFAULT_ACTION = "DefaultAction";
const std::string DEFAULT_LAYER  = "DefaultLayer";
}  // namespace
GuiManager::GuiManager()
{
    AddLayer(DEFAULT_LAYER);
}

GuiLayer& GuiManager::AddLayer(const std::string& name)
{
    auto iter = std::find_if(layers_.begin(), layers_.end(), [name](const auto& layer) { return layer.GetName() == name; });
    if (iter == layers_.end())
    {
        layers_.emplace_back(name);
        return layers_.back();
    }

    ERROR_LOG("Layer with name : " + name + " already exist");
    return *iter;
}

void GuiManager::Add(const std::string& layerName, std::unique_ptr<GuiElement> element)
{
    auto iter = std::find_if(layers_.begin(), layers_.end(), [layerName](const auto& layer) { return layer.GetName() == layerName; });

    if (iter != layers_.end())
    {
        iter->Add(std::move(element));
    }
    else
    {
        ERROR_LOG("Layer with name : " + layerName + " not found");
    }
}

void GuiManager::Add(std::unique_ptr<GuiElement> element)
{
    Add(DEFAULT_LAYER, std::move(element));
}
GuiElement* GuiManager::GetElement(const std::string& label)
{
    for (auto& layer : layers_)
    {
        auto element = std::find_if(layer.GetElements().begin(), layer.GetElements().end(), [&label](const auto& element) { return element->GetLabel() == label; });

        if (element != layer.GetElements().end())
        {
            return element->get();
        }
    }

    DEBUG_LOG("Element with label " + label + " not found");
    return nullptr;
}
GuiElement* GuiManager::GetElement(uint32 id)
{
    for (auto& layer : layers_)
    {
        auto element = std::find_if(layer.GetElements().begin(), layer.GetElements().end(), [id](const auto& element) { return element->GetId() == id; });

        if (element != layer.GetElements().end())
        {
            return element->get();
        }
    }
    return nullptr;
}
const GuiLayers& GuiManager::GetGuiLayers() const
{
    return layers_;
}

void GuiManager::Update()
{
    for (auto& layer : layers_)
    {
        for (auto iter = layer.GetElements().begin(); iter != layer.GetElements().end();)
        {
            auto element = iter->get();

            if (element and element->IsMarkToRemove())
            {
                iter = layer.GetElements().erase(iter);
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

    for (auto& layer : layers_)
    {
        auto iter = std::find_if(layer.GetElements().begin(), layer.GetElements().end(), [id](const auto& element) { return element->GetId() == id; });

        if (iter != layer.GetElements().end())
        {
            layer.GetElements().erase(iter);
        }
    }
}

void GuiManager::RemoveNotPermaments()
{
    DEBUG_LOG("");
    for (auto& layer : layers_)
    {
        for (auto iter = layer.GetElements().begin(); iter != layer.GetElements().end();)
        {
            if (not(*iter)->IsPermament())
            {
                DEBUG_LOG("Delete : " + std::to_string(iter->get()->GetId()));
                iter = layer.GetElements().erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }
}

void GuiManager::RemoveAll()
{
    for (auto& layer : layers_)
    {
        layer.GetElements().clear();
    }
}

GuiLayer* GuiManager::GetLayer(const std::string& name)
{
    auto iter = std::find_if(layers_.begin(), layers_.end(), [name](const auto& layer) { return layer.GetName() == name; });
    if (iter != layers_.end())
    {
        return &(*iter);
    }

    ERROR_LOG("Layer with name : " + name + " not found");
    return nullptr;
}

}  // namespace GameEngine
