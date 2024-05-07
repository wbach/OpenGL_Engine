#pragma once
#include "GameEngine/Components/BaseComponent.h"

namespace GameEngine
{
namespace Components
{
class Rigidbody;

class ArrowController : public BaseComponent
{
public:
    ArrowController(ComponentContext&, GameObject&);

    ArrowController& setDirection(const vec3&);
    void CleanUp() override;
    void ReqisterFunctions() override;

private:
    void update();

private:
    Rigidbody* rigidbody;
    vec3 direction;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
