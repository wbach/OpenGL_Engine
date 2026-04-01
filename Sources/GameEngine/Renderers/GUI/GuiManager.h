#pragma once
#include <functional>
#include <list>
#include <mutex>
#include <string>

#include "GuiAnimation.h"
#include "GuiElement.h"
#include "Input/KeyCodes.h"
#include "Layer/GuiLayer.h"
#include "Logger/Log.h"
#include "Text/GuiTextElement.h"
#include "Texutre/GuiTextureElement.h"
#include "magic_enum/magic_enum.hpp"

namespace GameEngine
{
typedef std::list<GuiLayer> GuiLayers;
typedef std::unordered_map<std::string, GuiElement*> GuiElementsMap;

using ActionFunction = std::function<void(GuiElement&)>;

class ENGINE_API GuiManager
{
public:
    GuiManager();
    ~GuiManager();
    GuiLayer& AddLayer(const std::string&);
    void Add(const std::string&, std::unique_ptr<GuiElement>);
    void Add(std::unique_ptr<GuiElement>);
    void add(GuiAnimation);
    void Update(float);
    void RegisterAction(const std::string&, ActionFunction);
    void RegisterDefaultAction(ActionFunction);
    bool SaveToFile(const File&);
    bool SaveToFile(const File&, const std::string&);
    void RemoveLayer(const std::string&);
    void RemoveAllFromLayer(const std::string&);
    void RemoveLayersExpect(const std::vector<std::string>& = {});
    void Remove(uint32);
    bool Remove(const GuiElement&);
    void RemoveAll();

    GuiLayer* GetLayer(const std::string&);
    const GuiLayers& GetGuiLayers() const;
    GuiLayers& GetGuiLayers();

    ActionFunction GetActionFunction(const std::string& name);

    void AddTask(std::function<void()>);

private:
    template <class T>
    T* Get(uint32);
    template <class T>
    T* Get(const std::string&);

    GuiElement* GetElement(const std::string&);
    GuiElement* GetElement(uint32 id);

private:
    GuiLayers layers_;
    std::mutex taskMutex_;
    std::mutex elementMutex_;
    std::unordered_map<std::string, ActionFunction> registeredActions_;
    std::vector<std::function<void()>> tasks_;
    std::vector<GuiAnimation> animations;
};

template <class T>
T* GuiManager::Get(uint32 id)
{
    auto element = GetElement(id);

    if (element)
    {
        if (element->GetType() == T::type)
        {
            return static_cast<T*>(element);
        }
        else
        {
            LOG_ERROR << "Can not get " << static_cast<int>(T::type)
                      << ", because element is type of :" << magic_enum::enum_name(element->GetType());
        }
    }
    else
    {
        LOG_ERROR << "Element with id : \"" << id << "\" not found.";
    }
    return nullptr;
}

template <class T>
T* GuiManager::Get(const std::string& label)
{
    auto element = GetElement(label);

    if (element)
    {
        if (element->GetType() == T::type)
        {
            return static_cast<T*>(element);
        }
        else
        {
            LOG_ERROR << "Can not get " << static_cast<int>(T::type)
                      << ", because element is type of :" << magic_enum::enum_name(element->GetType());
        }
    }
    return nullptr;
}

}  // namespace GameEngine
