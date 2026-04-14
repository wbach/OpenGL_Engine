#pragma once
#include "GameEngine/Renderers/GUI/Element.h"

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
}  // namespace GUI
}  // namespace GameEngine
