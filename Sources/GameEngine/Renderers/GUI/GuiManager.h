#pragma once
#include <string>
#include "GuiElement.h"
#include "Input/KeyCodes.h"
#include "Logger/Log.h"
#include "Text/GuiTextElement.h"
#include "Texutre/GuiTextureElement.h"
#include <functional>

namespace GameEngine
{
typedef std::vector<std::unique_ptr<GuiElement>> GuiElements;
typedef std::unordered_map<std::string, GuiElement*> GuiElementsMap;

using ActionFunction = std::function<void()>;

class GuiManager
{
public:
    GuiManager(std::function<void(GuiElement&)> renderSubscribe);
    template <class T>
    T* Get(const std::string& name);
    inline GuiElement* GetElement(const std::string& name);
    void Add(const std::string& name, std::unique_ptr<GuiElement> element);
    const GuiElements& GetElements() const;
    const GuiElementsMap& GetElementsMap() const;
    void Update();
    ActionFunction GetActionFunction(const std::string& name);
    void RegisterAction(const std::string&, ActionFunction);
    void SaveToFile(const std::string&);

private:
    GuiElementsMap elementsMap_;
    GuiElements elements_;
    std::function<void(GuiElement&)> subscribe_;
    std::unordered_map<std::string, ActionFunction> registeredActions_;
};

template <class T>
T* GuiManager::Get(const std::string& name)
{
    if (elementsMap_.count(name) > 0)
    {
        auto& element = elementsMap_.at(name);

        if (element->GetType() == T::type)
        {
            return static_cast<T*>(element);
        }
        else
        {
            ERROR_LOG("Can not get " + std::to_string(static_cast<int>(T::type)) +
                      ", because element is type of :" + std::to_string(static_cast<int>(element->GetType())));
        }
    }
    else
    {
        DEBUG_LOG("Element with name : \"" + name + "\" not found.");
    }
    return nullptr;
}
GuiElement* GuiManager::GetElement(const std::string& name)
{
    if (elementsMap_.count(name) > 0)
    {
        return elementsMap_.at(name);
    }
    else
    {
        DEBUG_LOG("Element with name : \"" + name + "\" not found.");
    }
    return nullptr;
}
}  // namespace GameEngine
