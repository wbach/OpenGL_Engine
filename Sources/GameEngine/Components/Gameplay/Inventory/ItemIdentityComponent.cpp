#include "ItemIdentityComponent.h"

#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "IdPool.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const char CSTR_ITEM_NAME[]   = "itemName";
const char CSTR_DESCRIPTION[] = "description";
const char CSTR_RARITY[]      = "rarity";
const char CSTR_GOLD_VALUE[]  = "goldValue";
const char CSTR_WEIGHT[]      = "weight";

static Utils::IdPool idPool;
}  // namespace

REGISTER_COMPONENT(ItemIdentityComponent)

ItemIdentityComponent::ItemIdentityComponent(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
    , id{idPool.getId()}
{
}

ItemIdentityComponent::ItemIdentityComponent(const ItemIdentityComponent& other)
    : Component(other)
    , itemName(other.itemName)
    , description(other.description)
    , rarity(other.rarity)
    , goldValue(other.goldValue)
    , weight(other.weight)
    , id{idPool.getId()}
{
}

ItemIdentityComponent::~ItemIdentityComponent()
{
    idPool.releaseId(id);
}

void ItemIdentityComponent::CleanUp()
{
}

void ItemIdentityComponent::Reload()
{
}

void ItemIdentityComponent::ReqisterFunctions()
{
}

void ItemIdentityComponent::read(const TreeNode& input)
{
    ::Read(input.getChild(CSTR_ITEM_NAME), itemName);
    ::Read(input.getChild(CSTR_DESCRIPTION), description);
    ::Read(input.getChild(CSTR_RARITY), rarity);
    ::Read(input.getChild(CSTR_GOLD_VALUE), goldValue);
    ::Read(input.getChild(CSTR_WEIGHT), weight);
}

void ItemIdentityComponent::write(TreeNode& node) const
{
    ::write(node.addChild(CSTR_ITEM_NAME), itemName);
    ::write(node.addChild(CSTR_DESCRIPTION), description);
    ::write(node.addChild(CSTR_RARITY), rarity);
    ::write(node.addChild(CSTR_GOLD_VALUE), goldValue);
    ::write(node.addChild(CSTR_WEIGHT), weight);
}
IdType ItemIdentityComponent::getId() const
{
    return id;
}
}  // namespace Components
}  // namespace GameEngine