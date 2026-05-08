#pragma once
#include <string>

#include "GameEngine/Components/Component.h"
#include "Types.h"

namespace GameEngine
{
enum class Rarity : uint8_t
{
    Common,
    Uncommon,
    Rare,
    Epic,
    Legendary
};

namespace Components
{
DECLARE_COMPONENT(ItemIdentityComponent)
{
public:
    std::string itemName    = "Unknown Item";
    std::string description = "";
    Rarity rarity           = Rarity::Common;
    int goldValue           = 0;
    float weight            = 0.0f;

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
    ItemIdentityComponent(const ItemIdentityComponent&);
    ~ItemIdentityComponent() override;

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    IdType getId() const;

private:
    IdType id;

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
