#pragma once
#include "CollisionShape.h"

namespace GameEngine
{
namespace Components
{
class SphereShape : public CollisionShape
{
public:
    SphereShape(const ComponentContext& componentContext, GameObject& gameObject);
    void Update();
    virtual void ReqisterFunctions() override;
    void OnAwake();

public:
    SphereShape& SetSize(float size);
    inline float GetSize() const;

private:
    float size_;

public:
    static ComponentsType type;
};
float SphereShape::GetSize() const
{
    return size_;
}
}  // namespace Components
}  // namespace GameEngine
