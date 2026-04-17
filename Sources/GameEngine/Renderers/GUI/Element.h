#pragma once
#include <Input/KeyCodes.h>
#include <Types.h>

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "IElementVisitor.h"
#include "Spacing.h"
#include "Transform.h"

namespace GameEngine
{
namespace GUI
{
using Label = std::string;

class ENGINE_API Element
{
    using Children = std::vector<std::unique_ptr<Element>>;

public:
    Element();
    virtual ~Element();

    Element(const Element&);
    Element& operator=(const Element&);

public:
    virtual void update();
    virtual void onMouseClick(const vec2&, KeyCodes::Type);
    virtual void onMouseClickRelease(KeyCodes::Type);
    virtual void onMouseEnter();
    virtual void onMousePositionUpdate(const vec2&);
    virtual void onMouseLeave();
    virtual bool onMouseWheel(const vec2&);
    virtual void accept(IElementVisitor&);
    virtual std::unique_ptr<Element> clone() const;

    void setParent(Element*);
    Element* getParent();
    void addChild(std::unique_ptr<Element>);
    const Children& getChildren() const;

    void reorderChildUp(const Element&);
    void reorderChildDown(const Element&);

    bool removeChild(IdType);
    void removeAll();

    void setLocalScale(const vec2&);
    void setLocalPosition(const vec2&);
    void setInternalPosition(const vec2&);
    void setScreenScale(const vec2&);
    void setScreenPostion(const vec2&);
    void setTransform(const Transform&);
    void invalidate();
    void refresh();

    void setMargin(const Spacing&);
    const Spacing& getMargin() const;

    void activate(bool);
    void activateInternal(bool);
    bool isActive() const;

    void setLabel(const std::string&);
    void setZPosition(float);

    const vec2& getLocalScale() const;
    const vec2& getLocalPosition() const;
    vec2 getScreenScale() const;
    vec2 getScreenPosition() const;
    float getZValue() const;
    Element* get(const Label&);
    Element* get(IdType);
    const Label& getLabel() const;
    bool compareZValue(const Element&) const;

    template <typename T>
    T* get(const Label& label)
    {
        if (auto element = get(label))
        {
            return dynamic_cast<T*>(element);
        }
        return nullptr;
    }

    template <typename T>
    T* get(IdType id)
    {
        if (auto element = get(id))
        {
            return dynamic_cast<T*>(element);
        }
        return nullptr;
    }

protected:
    virtual void refreshSelf();

public:
    IdType getId() const;
    const Transform& getTransform() const;

protected:
    Spacing margin_;
    Element* parent;
    Children children;
    Transform transform;
    Label label;
    bool mouseFocused{false};
    bool isDirty{true};

    bool active;
    IdType id;
};
}  // namespace GUI
}  // namespace GameEngine
