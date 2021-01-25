
#pragma once
#include <Types.h>

#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include "GLM/GLMUtils.h"
#include "GuiElementTypes.h"
#include "Rect.h"

namespace GameEngine
{
struct GuiElementTransform
{
    float zValue{0.f};
    vec2 position{0.5f};
    vec2 scale{1.0f};
};

class GuiElement
{
public:
    GuiElement(GuiElementTypes);
    virtual ~GuiElement() = default;

public:
    virtual void AddChild(std::unique_ptr<GuiElement>);
    const std::vector<std::unique_ptr<GuiElement>>& GetChildren() const;
    bool RemoveChild(uint32 id);
    void RemoveAll();

    virtual void Update();
    virtual bool IsCollision(const vec2&) const;
    virtual std::optional<vec2> GetCollisionPoint(const vec2&) const;
    virtual void SetLocalScale(const vec2&);
    virtual void SetLocalPostion(const vec2&);
    virtual void SetScreenScale(const vec2&);
    virtual void SetScreenPostion(const vec2&);
    virtual void Show(bool);
    virtual void Show();
    virtual void ShowPartial(uint32 depth);
    virtual void Hide();
    virtual void Activate();
    virtual void Deactivate();
    virtual bool IsActive() const;

    virtual const vec2& GetLocalScale() const;
    virtual const vec2& GetLocalPosition() const;

    virtual vec2 GetScreenScale() const;
    virtual vec2 GetScreenPosition() const;

    virtual void SetZPosition(float z);
    virtual float GetZValue() const;
    virtual void SetIsInternal();
    virtual void SetIsInternal(bool);
    virtual bool IsInternal() const;

    virtual bool CompareZValue(const GuiElement& element) const;
    virtual GuiElement* GetCollisonElement(const vec2& mousePosition);
    virtual GuiElement* Get(const std::string& label);
    virtual GuiElement* GetChild(uint32);

    virtual void setParent(GuiElement*);

protected:
    void CallOnChange();

public:
    GuiElementTypes GetType() const;
    uint32 GetId() const;
    bool IsShow() const;

    const std::string& GetLabel() const;
    void SetLabel(const std::string& label);

    void SetStartupFunctionName(const std::string&);
    const std::string& GetStartupFunctionName() const;

    void EnableChangeNotif();
    void DisableChangeNotif();

    const GuiElementTransform& getTransform() const;
    void SubscribeForChange(std::function<void()>);

private:
    GuiElementTypes type_;
    std::vector<std::function<void()>> changeSubscribers_;
    bool changeNotif_;

protected:
    GuiElement* parent_;
    std::vector<std::unique_ptr<GuiElement>> children_;
    std::string startupFunctionName_;

    std::string label_;

    GuiElementTransform transform_;

    bool show_;
    bool isActive_;
    bool isInternal_;

    uint32 id_;

public:
    static uint32 ID;
};
}  // namespace GameEngine
