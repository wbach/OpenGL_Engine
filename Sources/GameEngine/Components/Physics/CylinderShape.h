#pragma once
#include "CollisionShape.h"

namespace GameEngine
{
namespace Components
{
class CylinderShape : public CollisionShape
{
public:
    CylinderShape(ComponentContext&, GameObject&);
    void ReqisterFunctions() override;
    void OnAwake();

public:
    CylinderShape& SetSize(const vec3&);
    const vec3& GetSize() const { return size_; }

private:
    vec3 size_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
    static std::string name;
};
}  // namespace Components
}  // namespace GameEngine
