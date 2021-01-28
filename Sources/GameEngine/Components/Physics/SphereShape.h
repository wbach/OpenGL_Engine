#pragma once
#include "CollisionShape.h"

namespace GameEngine
{
namespace Components
{
class SphereShape : public CollisionShape
{
public:
    SphereShape(ComponentContext& componentContext, GameObject& gameObject);
    void CleanUp() override;
    void ReqisterFunctions() override;
    void Update();
    void OnAwake();

public:
    SphereShape& SetRadius(float size);
    inline float GetRadius() const;

private:
    float radius_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
    static std::string name;
};
float SphereShape::GetRadius() const
{
    return radius_;
}
}  // namespace Components
}  // namespace GameEngine
