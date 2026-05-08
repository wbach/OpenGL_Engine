#pragma once
#include <optional>

#include "GameEngine/Components/Component.h"

namespace GameEngine
{
namespace Components
{
class CharacterStatsComponent;
class CharacterController;

DECLARE_COMPONENT(HealthComponent)
{
public:
    // clang-format off
    BEGIN_FIELDS()
    END_FIELDS()
    // clang-format on

public:
    HealthComponent(ComponentContext&, GameObject&);
    ~HealthComponent() override;

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    void takeDamage(float);
    void heal(float);

private:
    bool canProccess() const;

private:
    CharacterStatsComponent* stats{nullptr};
    CharacterController* controller{nullptr};

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
