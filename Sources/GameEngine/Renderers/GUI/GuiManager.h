#pragma once
#include <functional>
#include <string>
#include "GuiElement.h"
#include "Input/KeyCodes.h"
#include "Logger/Log.h"
#include "Text/GuiTextElement.h"
#include "Texutre/GuiTextureElement.h"

namespace GameEngine
{
typedef std::vector<std::unique_ptr<GuiElement>> GuiElements;
typedef std::unordered_map<std::string, GuiElement*> GuiElementsMap;

using ActionFunction = std::function<void()>;

class GuiManager
{
public:
    GuiManager(std::function<void(GuiElement&)> renderSubscribe, std::function<void(const GuiElement&)> unsubscribeElement, std::function<void()> unsubscribeAll);

    void Add(std::unique_ptr<GuiElement> element);
    void Update();
    void RegisterAction(const std::string&, ActionFunction);
    bool SaveToFile(const std::string&);
    void Remove(uint32);
    void Remove(const GuiElement&);

    template <class T>
    T* Get(uint32);
    template <class T>
    T* Get(const std::string&);

    GuiElement* GetElement(const std::string&);
    GuiElement* GetElement(uint32 id);
    const GuiElements& GetElements() const;
    ActionFunction GetActionFunction(const std::string& name);

private:
    GuiElements elements_;

    std::function<void(GuiElement&)> subscribe_;
    std::function<void()> unsubscribeAll_;
    std::function<void(const GuiElement&)> unsubscribeElement_;

    std::unordered_map<std::string, ActionFunction> registeredActions_;
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
