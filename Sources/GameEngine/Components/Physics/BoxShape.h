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
    void OnAwake();

public:
    BoxShape& SetSize(const vec3& size);
    BoxShape& SetSize(float size);

private:
    vec3 size_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
