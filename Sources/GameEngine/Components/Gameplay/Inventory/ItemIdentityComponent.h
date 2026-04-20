#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include <string>

namespace GameEngine
{
enum class Rarity : uint8_t {
    Common,
    Uncommon,
    Rare,
    Epic,
    Legendary
};

namespace Components
{
class ENGINE_API ItemIdentityComponent : public BaseComponent
{
public:
    std::string itemName = "Unknown Item";
    std::string description = "";
    Rarity rarity = Rarity::Common;
    int goldValue = 0;
    float weight = 0.0f;

    // clang-format off
    BEGIN_FIELDS()
        FIELD_STRING(itemName)
        FIELD_STRING(description)
        FIELD_ENUM(rarity)
        FIELD_INT(goldValue)
        FIELD_FLOAT(weight)
    END_FIELDS()
    // clang-format on

public:
    ItemIdentityComponent(ComponentContext&, GameObject&);
    ~ItemIdentityComponent() override;

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
