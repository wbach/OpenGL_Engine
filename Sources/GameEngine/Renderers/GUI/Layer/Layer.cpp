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
}
Layer::Layer(const std::string& name)
    : name(name)
{
}

void Layer::activate(bool v)
{
    for (auto& [_, rootElement] : groups)
        rootElement.activate(v);
}

void Layer::activate(std::string_view name, bool v)
{
    if (auto iter = groups.find(name); iter != groups.end())
        iter->second.activate(v);
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

bool Layer::isActive(std::string_view name) const
{
    if (auto iter = groups.find(name); iter != groups.end())
        return iter->second.isActive();

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

const Layer::LayerGroups& Layer::get() const
{
    return groups;
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
    groups.clear();
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

    return createGroup(name);
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

Element* Layer::getGroup(std::string_view name)
{
    if (auto iter = groups.find(name); iter != groups.end())
        return &iter->second;

    return nullptr;
}
Element& Layer::createGroup(std::string_view name)
{
    LOG_DEBUG << "Layer " << name << " add new group " << name;
    auto [it, inserted] = groups.try_emplace(std::string{name}, Element{});
    return it->second;
}
}  // namespace GUI
}  // namespace GameEngine
