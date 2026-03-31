#include "GuiManager.h"

#include <Logger/Log.h>

#include <algorithm>

#include "GuiElementWriter.h"
#include "GuiElementsDef.h"

namespace GameEngine
{
GuiManager::GuiManager()
{
    AddLayer(Gui::DEFAULT_LAYER);
}

GuiManager::~GuiManager()
{
    LOG_DEBUG << "destructor";
}

GuiLayer& GuiManager::AddLayer(const std::string& name)
{
    auto iter = std::find_if(layers_.begin(), layers_.end(), [name](const auto& layer) { return layer.GetName() == name; });
    if (iter == layers_.end())
    {
        layers_.emplace_back(name);
        return layers_.back();
    }

    LOG_WARN << "Layer with name : " << name << " already exist";
    return *iter;
}

void GuiManager::Add(const std::string& layerName, std::unique_ptr<GuiElement> element)
{
    std::lock_guard<std::mutex> lk(elementMutex_);
    auto iter =
        std::find_if(layers_.begin(), layers_.end(), [layerName](const auto& layer) { return layer.GetName() == layerName; });

    if (iter != layers_.end())
    {
        iter->Add(std::move(element));
    }
    else
    {
        LOG_WARN << "Layer with name : " << layerName << " not found";
    }
}

void GuiManager::Add(std::unique_ptr<GuiElement> element)
{
    Add(Gui::DEFAULT_LAYER, std::move(element));
}
void GuiManager::add(GuiAnimation element)
{
    animations.push_back(std::move(element));
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

    LOG_WARN << "Element with label " << label << " not found";
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
GuiLayers& GuiManager::GetGuiLayers()
{
    return layers_;
}
void GuiManager::Update(float deltaTime)
{
    for (auto iter = animations.begin(); iter != animations.end();)
    {
        if (iter->update(deltaTime) == GuiAnimation::Status::done)
        {
            iter = animations.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    {
        std::lock_guard<std::mutex> lk(elementMutex_);
        for (auto& layer : layers_)
        {
            for (auto& element : layer.GetElements())
            {
                element->Update();
            }
        }
    }

    std::vector<std::function<void()>> tmpTasks;
    {
        std::lock_guard<std::mutex> lk(taskMutex_);
        tmpTasks = std::move(tasks_);
    }

    for (auto& task : tmpTasks)
    {
        task();
    }
}

ActionFunction GuiManager::GetActionFunction(const std::string& name)
{
    if (registeredActions_.count(name))
    {
        return registeredActions_.at(name);
    }
    if (registeredActions_.count(Gui::DEFAULT_ACTION))
    {
        return registeredActions_.at(Gui::DEFAULT_ACTION);
    }
    return [](auto&) { LOG_WARN << "Button action not found. Default action not set."; };
}

void GuiManager::AddTask(std::function<void()> task)
{
    std::lock_guard<std::mutex> lk(taskMutex_);
    tasks_.push_back(task);
}

void GuiManager::RegisterAction(const std::string& name, ActionFunction action)
{
    registeredActions_.insert({name, action});
}

void GuiManager::RegisterDefaultAction(ActionFunction action)
{
    registeredActions_.insert({Gui::DEFAULT_ACTION, action});
}

bool GuiManager::SaveToFile(const File& file)
{
    return SaveToFile(file, Gui::DEFAULT_LAYER);
}

bool GuiManager::SaveToFile(const File& file, const std::string& layerName)
{
    auto layer = GetLayer(layerName);
    if (layer)
    {
        GuiElementWriter::write(file, *layer);
        return true;
    }

    return false;
}

void GuiManager::RemoveLayersExpect(const std::vector<std::string>& exceptions)
{
    std::lock_guard<std::mutex> lk(elementMutex_);
    if (exceptions.empty())
    {
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
    std::lock_guard<std::mutex> lk(elementMutex_);
    auto element = GetElement(id);
    if (element)
    {
        for (auto& layer : layers_)
        {
            if (layer.removeElement(*element))
                return;
        }
    }
    else
    {
        LOG_WARN << "Element not found";
    }
}

bool GuiManager::Remove(const GuiElement& element)
{
    std::lock_guard<std::mutex> lk(elementMutex_);
    for (auto& layer : layers_)
    {
        if (layer.removeElement(element))
            return true;
    }

    return false;
}

void GuiManager::RemoveAll()
{
    std::lock_guard<std::mutex> lk(elementMutex_);
    for (auto& layer : layers_)
    {
        layer.clear();
    }
}

GuiLayer* GuiManager::GetLayer(const std::string& name)
{
    std::lock_guard<std::mutex> lk(elementMutex_);
    auto iter = std::find_if(layers_.begin(), layers_.end(), [name](const auto& layer) { return layer.GetName() == name; });
    if (iter != layers_.end())
    {
        return &(*iter);
    }

    LOG_ERROR << "Layer with name : " << name << " not found";
    return nullptr;
}

void GuiManager::RemoveLayer(const std::string& layerName)
{
    std::lock_guard<std::mutex> lk(elementMutex_);
    auto iter =
        std::find_if(layers_.begin(), layers_.end(), [layerName](const auto& layer) { return layer.GetName() == layerName; });

    if (iter != layers_.end())
    {
        layers_.erase(iter);
    }
    else
    {
        LOG_WARN << "Layer with name : " << layerName << " not found";
    }
}
}  // namespace GameEngine
