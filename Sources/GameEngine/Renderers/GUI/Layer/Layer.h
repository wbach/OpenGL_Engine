#pragma once
#include <Logger/Log.h>
#include <Types.h>

#include <string>
#include <string_view>
#include <unordered_map>

#include "GameEngine/Renderers/GUI/Element.h"
namespace GameEngine
{
namespace GUI
{
inline constexpr std::string_view defaultGroupName{"defaultGroupName"};

class ENGINE_API Layer
{
public:
    using LayerGroups = std::unordered_map<std::string, Element, StringViewHash, std::equal_to<>>;

    Layer(std::string_view);
    Layer(const std::string&);

    void activate(bool);
    void activate(std::string_view, bool);
    bool isActive() const;
    bool isActive(std::string_view) const;
    void add(std::unique_ptr<Element>);
    void add(std::string_view, std::unique_ptr<Element>);
    Element& createGroup(std::string_view);
    void setZPosition(float);
    bool removeElement(const Element&);
    void refresh();
    void clear();

    const std::string& getName() const;
    const LayerGroups& get() const;
    Element* get(std::string_view, const Label&);
    Element* get(std::string_view, IdType);
    Element* get(const Label&);
    Element* get(IdType);
    Element* getGroup(std::string_view);

    template <typename T>
    T* getTypedElement(const std::string& name)
    {
        return getTypedElement<T>(defaultGroupName, name);
    }

    template <typename T>
    T* getTypedElement(std::string_view, const std::string& name)
    {
        auto element = get(name);
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

    Element& getOrCreateGroup(std::string_view);

private:
    std::string name;

    LayerGroups groups;

private:
    static uint32 LAYERID;
};
}  // namespace GUI
}  // namespace GameEngine
