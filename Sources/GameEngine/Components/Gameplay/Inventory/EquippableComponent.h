#pragma once
#include "GameEngine/Components/ComponentCore.h"
#include "SlotType.h"

namespace GameEngine
{
namespace Components
{
class ENGINE_API EquippableComponent : public ComponentCore
{
public:
    SlotType slot = SlotType::None;

    int strReq = 0;
    int dexReq = 0;
    int intReq = 0;

    // clang-format off
    BEGIN_FIELDS()
        FIELD_ENUM(slot)
        FIELD_INT(strReq)
        FIELD_INT(dexReq)
        FIELD_INT(intReq)
    END_FIELDS()
    // clang-format on

public:
    EquippableComponent(ComponentContext&, GameObject&);
    ~EquippableComponent() override;

public:
    bool isEquipped{false};

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
