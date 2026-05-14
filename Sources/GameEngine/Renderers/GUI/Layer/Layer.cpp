#include "Layer.h"

#include <cassert>
#include <string>
#include <string_view>

#include "GameEngine/Renderers/GUI/Element.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace GUI
{
Layer::Layer(std::string_view name)
    : name(name)
{
    groups.try_emplace(std::string{defaultGroupName}, Element{});
}
Layer::Layer(const std::string& name)
    : name(name)
{
    groups.try_emplace(std::string{defaultGroupName}, Element{});
}

void Layer::activate(bool v)
{
    for (auto& [_, rootElement] : groups)
        rootElement.activate(v);
}

bool Layer::isActive() const
{
    for (auto& [_, rootElement] : groups)
    {
        if (rootElement.isActive())
        {
            return true;
        }
    }

    return false;
}

void Layer::add(std::unique_ptr<Element> element)
{
    add(defaultGroupName, std::move(element));
}
void Layer::add(std::string_view group, std::unique_ptr<Element> element)
{
    getOrCreateGroup(group).addChild(std::move(element));
}
const std::vector<std::unique_ptr<Element>>& Layer::get() const
{
    if (auto iter = groups.find(defaultGroupName); iter != groups.end())
        return iter->second.getChildren();

    LOG_WARN << "defaultGroupName not exist";

    static std::vector<std::unique_ptr<Element>> el;
    return el;
}

const std::string& Layer::getName() const
{
    return name;
}

void Layer::setZPosition(float z)
{
    for (auto& [_, rootElement] : groups)
    {
        rootElement.setZPosition(z);
    }
}
bool Layer::removeElement(const Element& element)
{
    for (auto& [_, rootElement] : groups)
    {
        if (rootElement.removeChild(element.getId()))
        {
            return true;
        }
    }
    return false;
}
void Layer::clear()
{
    for (auto& [_, rootElement] : groups)
    {
        rootElement.removeAll();
    }
}
Element* Layer::get(const Label& label)
{
    for (auto& [_, rootElement] : groups)
    {
        if (auto element = rootElement.get(label))
        {
            return element;
        }
    }

    return nullptr;
}
Element* Layer::get(IdType id)
{
    for (auto& [_, rootElement] : groups)
    {
        if (auto element = rootElement.get(id))
        {
            return element;
        }
    }
    return nullptr;
}
void Layer::refresh()
{
    for (auto& [_, rootElement] : groups)
    {
        rootElement.refresh();
    }
}
Element& Layer::getOrCreateGroup(std::string_view name)
{
    if (auto it = groups.find(name); it != groups.end())
    {
        return it->second;
    }

    auto [it, inserted] = groups.try_emplace(std::string{name}, Element{});
    return it->second;
}
Element* Layer::get(std::string_view group, const Label& label)
{
    if (auto it = groups.find(name); it != groups.end())
    {
        return it->second.get(label);
    }
    return nullptr;
}
Element* Layer::get(std::string_view group, IdType id)
{
    if (auto it = groups.find(name); it != groups.end())
    {
        return it->second.get(id);
    }
    return nullptr;
}

}  // namespace GUI
}  // namespace GameEngine
