#pragma once
#include "GameEngine/Components/BaseComponent.h"

namespace GameEngine
{
enum class SlotType : uint8_t
{
    None,
    MainHand,
    OffHand,
    BothHands,
    Head,
    Chest,
    Legs,
    Feet,
    Finger,
    Neck
};

namespace Components
{
class ENGINE_API EquippableComponent : public BaseComponent
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

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
