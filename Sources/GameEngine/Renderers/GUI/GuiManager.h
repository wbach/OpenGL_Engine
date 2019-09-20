#pragma once
#include <functional>
#include <string>
#include "GuiElement.h"
#include "Input/KeyCodes.h"
#include "Logger/Log.h"
#include "Text/GuiTextElement.h"
#include "Texutre/GuiTextureElement.h"
#include "Layer/GuiLayer.h"

namespace GameEngine
{
typedef std::vector<GuiLayer> GuiLayers;
typedef std::unordered_map<std::string, GuiElement*> GuiElementsMap;

using ActionFunction = std::function<void(GuiElement&)>;

class GuiManager
{
public:
    GuiManager();
    GuiLayer& AddLayer(const std::string&);
    void Add(const std::string& layerName, std::unique_ptr<GuiElement> element);
    void Add(std::unique_ptr<GuiElement> element);
    void Update();
    void RegisterAction(const std::string&, ActionFunction);
    void RegisterDefaultAction(ActionFunction);
    bool SaveToFile(const std::string&);
    void Remove(uint32);
    void Remove(const GuiElement&);
    void RemoveNotPermaments();
    void RemoveAll();

    template <class T>
    T* Get(uint32);
    template <class T>
    T* Get(const std::string&);

    GuiLayer* GetLayer(const std::string&);
    const GuiLayers& GetGuiLayers() const;

    GuiElement* GetElement(const std::string&);
    GuiElement* GetElement(uint32 id);
    ActionFunction GetActionFunction(const std::string& name);

    void AddTask(std::function<void()>);

private:
    GuiLayers layers_;
    std::unordered_map<std::string, ActionFunction> registeredActions_;
    std::vector<std::function<void()>> tasks_;
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
            ERROR_LOG("Can not get " + std::to_string(static_cast<int>(T::type)) + ", because element is type of :" + std::to_string(static_cast<int>(element->GetType())));
        }
    }
    else
    {
        DEBUG_LOG("Element with id : \"" + std::to_string(id) + "\" not found.");
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
            ERROR_LOG("Can not get " + std::to_string(static_cast<int>(T::type)) + ", because element is type of :" + std::to_string(static_cast<int>(element->GetType())));
        }
    }
    return nullptr;
}

}  // namespace GameEngine
