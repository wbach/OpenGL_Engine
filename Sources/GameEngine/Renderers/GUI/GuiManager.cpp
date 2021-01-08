#include "GuiManager.h"

#include <algorithm>

#include "GuiElementWriter.h"

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

GuiManager::~GuiManager()
{
    DEBUG_LOG("destructor");
}

GuiLayer& GuiManager::AddLayer(const std::string& name)
{
    auto iter =
        std::find_if(layers_.begin(), layers_.end(), [name](const auto& layer) { return layer.GetName() == name; });
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
    auto iter = std::find_if(layers_.begin(), layers_.end(),
                             [layerName](const auto& layer) { return layer.GetName() == layerName; });

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
        auto element = std::find_if(layer.GetElements().begin(), layer.GetElements().end(),
                                    [&label](const auto& element) { return element->Get(label) != nullptr; });

        if (element != layer.GetElements().end())
        {
            return (*element)->Get(label);
        }
    }

    DEBUG_LOG("Element with label " + label + " not found");
    return nullptr;
}
GuiElement* GuiManager::GetElement(uint32 id)
{
    for (auto& layer : layers_)
    {
        auto element = std::find_if(layer.GetElements().begin(), layer.GetElements().end(),
                                    [id](const auto& element) { return element->GetId() == id; });

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
        for (auto& element : layer.GetElements())
        {
            element->Update();
        }
    }

    std::lock_guard<std::mutex> lk(taskMutex_);
    for (auto& task : tasks_)
    {
        task();
    }
    tasks_.clear();
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
    std::lock_guard<std::mutex> lk(taskMutex_);
    tasks_.push_back(task);
}

void GuiManager::AddRemoveTask(GuiElement* element)
{
    std::lock_guard<std::mutex> lk(taskMutex_);
    tasks_.push_back([this, element]() { Remove(element->GetId()); });
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
    return SaveToFile(filename, DEFAULT_LAYER);
}

bool GuiManager::SaveToFile(const std::string& filename, const std::string& layerName)
{
    auto layer = GetLayer(layerName);
    if (layer)
    {
        GuiElementWriter::write(filename, *layer);
        return true;
    }

    return false;
}

void GuiManager::RemoveLayersExpect(const std::vector<std::string>& exceptions)
{
    DEBUG_LOG("Remove ");
    if (exceptions.empty())
    {
        DEBUG_LOG("Remove all");
        layers_.clear();
    }
    else
    {
        for (auto iter = layers_.begin(); iter != layers_.end();)
        {
            const auto& layerName = iter->GetName();
            auto skip             = std::find_if(exceptions.begin(), exceptions.end(),
                                     [&layerName](const auto& layer) { return layerName == layer; });

            if (skip == exceptions.end())
            {
                DEBUG_LOG("Remove " + layerName);
                iter = layers_.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }
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
        auto iter = std::find_if(layer.GetElements().begin(), layer.GetElements().end(),
                                 [id](const auto& element) { return element->GetId() == id; });

        if (iter != layer.GetElements().end())
        {
            layer.GetElements().erase(iter);
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
    auto iter =
        std::find_if(layers_.begin(), layers_.end(), [name](const auto& layer) { return layer.GetName() == name; });
    if (iter != layers_.end())
    {
        return &(*iter);
    }

    ERROR_LOG("Layer with name : " + name + " not found");
    return nullptr;
}

}  // namespace GameEngine
