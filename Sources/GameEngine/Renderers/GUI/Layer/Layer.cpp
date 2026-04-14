#include "Layer.h"

namespace GameEngine
{
namespace GUI
{
Layer::Layer(const std::string& name)
    : name(name)
{
}

void Layer::activate(bool v)
{
    rootElement_.activate(v);
}

bool Layer::isActive() const
{
    return rootElement_.isActive();
}

void Layer::add(std::unique_ptr<Element> element)
{
    rootElement_.addChild(std::move(element));
}
const std::vector<std::unique_ptr<Element>>& Layer::get() const
{
    return rootElement_.getChildren();
}

const std::string& Layer::getName() const
{
    return name;
}

void Layer::setZPosition(float z)
{
    rootElement_.setZPosition(z);
}
bool Layer::removeElement(const Element& element)
{
    return rootElement_.removeChild(element.getId());
}
void Layer::clear()
{
    rootElement_.removeAll();
}
Element* Layer::get(const Label& label)
{
    return rootElement_.get(label);
}
Element* Layer::get(IdType id)
{
    return rootElement_.get(id);
}
void Layer::refresh()
{
    rootElement_.refresh();
}
}  // namespace GUI
}  // namespace GameEngine
