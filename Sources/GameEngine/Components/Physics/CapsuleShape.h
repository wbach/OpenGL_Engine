#pragma once
#include "CollisionShape.h"

namespace GameEngine
{
namespace Components
{
class CapsuleShape : public CollisionShape
{
public:
    CapsuleShape(ComponentContext& componentContext, GameObject& gameObject);
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
    static void registerReadFunctions();
    void write(TreeNode&) const override;
    static const std::string name;
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
