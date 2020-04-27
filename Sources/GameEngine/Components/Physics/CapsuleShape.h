#pragma once
#include "CollisionShape.h"

namespace GameEngine
{
namespace Components
{
class CapsuleShape : public CollisionShape
{
public:
    CapsuleShape(const ComponentContext& componentContext, GameObject& gameObject);
    void CleanUp() override;
    void ReqisterFunctions() override;

    void Update();
    void OnAwake();

public:
    CapsuleShape& SetHeight(float h);
    CapsuleShape& SetRadius(float r);
    inline float GetHeight() const;
    inline float GetRadius() const;

private:
    float height_;
    float radius_;

public:
    static ComponentsType type;
};

float CapsuleShape::GetHeight() const
{
    return height_;
}
inline float CapsuleShape::GetRadius() const
{
    return radius_;
}
}  // namespace Components
}  // namespace GameEngine
