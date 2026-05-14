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
    Layer(std::string_view);
    Layer(const std::string&);

    void activate(bool);
    bool isActive() const;
    void add(std::unique_ptr<Element>);
    void add(std::string_view, std::unique_ptr<Element>);
    void setZPosition(float);
    bool removeElement(const Element&);
    void refresh();
    void clear();

    const std::string& getName() const;
    const std::vector<std::unique_ptr<Element>>& get() const;
    Element* get(std::string_view, const Label&);
    Element* get(std::string_view, IdType);
    Element* get(const Label&);
    Element* get(IdType);

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

    using LayerGroups = std::unordered_map<std::string, Element, StringViewHash, std::equal_to<>>;
    LayerGroups groups;

private:
    static uint32 LAYERID;
};
}  // namespace GUI
}  // namespace GameEngine
