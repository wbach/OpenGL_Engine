
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
class GuiElement
{
    struct ZValue
    {
        float value   = 0.f;
        float offset_ = 0.f;
        float total_  = 0.f;
    };

public:
    GuiElement(GuiElementTypes type, const WindowSize& windowSize);
    virtual ~GuiElement() = default;

public:
    virtual void AddChild(std::unique_ptr<GuiElement>);
    const std::vector<std::unique_ptr<GuiElement>>& GetChildren() const;
    void RemoveChild(uint32 id);
    void RemoveAll();

    virtual void Update();
    virtual bool IsCollision(const vec2&) const;
    virtual std::optional<vec2> GetCollisionPoint(const vec2&) const;
    virtual void SetScale(const vec2& scale);
    virtual void SetPostion(const vec2& position);
    virtual void Show(bool);
    virtual void Show();
    virtual void ShowPartial(uint32 depth);
    virtual void Hide();
    virtual void Activate();
    virtual void Deactivate();
    virtual bool IsActive() const;

    virtual const vec2& GetScale() const;
    virtual const vec2& GetPosition() const;

    virtual void SetZPosition(float z);
    virtual void SetZPositionOffset(float offset);
    virtual float GetZValue() const;
    virtual void SetIsInternal();
    virtual void SetIsInternal(bool);
    virtual bool IsInternal() const;

    virtual bool CompareZValue(const GuiElement& element) const;
    virtual GuiElement* GetCollisonElement(const vec2& mousePosition);
    virtual GuiElement* Get(const std::string& label);
    virtual GuiElement* GetChild(uint32);

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

protected:
    void UpdatePosition(GuiElement& element, const vec2& v);
    void SubscribeForChange(std::function<void()>);

private:
    GuiElementTypes type_;
    std::vector<std::function<void()>> changeSubscribers_;
    bool changeNotif_;

protected:
    std::vector<std::unique_ptr<GuiElement>> children_;
    std::string startupFunctionName_;

    std::string label_;
    WindowSize windowSize_;
    vec2 position_;
    ZValue zPosition_;
    vec2 scale_;

    bool show_;
    bool isActive_;
    bool isInternal_;

    uint32 id_;

public:
    static uint32 ID;
};
}  // namespace GameEngine
