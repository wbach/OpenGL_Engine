#pragma once
#include "CollisionShape.h"

namespace GameEngine
{
namespace Components
{
class BoxShape : public CollisionShape
{
public:
    BoxShape(const ComponentContext& componentContext, GameObject& gameObject);
    void Update();
    virtual void ReqisterFunctions() override;

public:
    BoxShape& SetSize(const vec3& size);
    BoxShape& SetSize(float size);
    const vec3& GetSize() const { return size_; }

private:
    void OnAwake();

private:
    vec3 size_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
