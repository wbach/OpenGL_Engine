#pragma once
#include "GameEngine/Renderers/GUI/Element.h"
#include <Logger/Log.h>
namespace GameEngine
{
namespace GUI
{
class ENGINE_API Layer
{
public:
    Layer(const std::string&);

    void activate(bool);
    bool isActive() const;
    void add(std::unique_ptr<Element>);
    void setZPosition(float);
    bool removeElement(const Element&);
    void refresh();
    void clear();

    const std::string& getName() const;
    const std::vector<std::unique_ptr<Element>>& get() const;
    Element* get(const Label&);
    Element* get(IdType);

private:
    std::string name;
    Element rootElement_;

private:
    static uint32 LAYERID;
};

template <typename T>
T* getTypedElement(GUI::Layer* layer, const std::string& name)
{
    auto element = layer->get(name);
    if (not element)
    {
        LOG_WARN << "Element not found: " << name;
        return nullptr;
    }
    auto typed = dynamic_cast<T*>(element);
    if (not typed)
    {
        LOG_WARN << "Wrong element type for: " << name;
        return nullptr;
    }
    return typed;
}
}  // namespace GUI
}  // namespace GameEngine
