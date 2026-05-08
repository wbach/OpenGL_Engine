#pragma once

#include <memory>

#include "GameEngine/Components/Component.h"

class TreeNode;

namespace GameEngine
{
namespace Components
{
DECLARE_COMPONENT(UnknownExternalComponent)
{
public:
    UnknownExternalComponent(ComponentContext&, GameObject&);
    std::string GetOrginalComponentName() const ;

private:
    std::unique_ptr<TreeNode> orginalNode;

public:
    void read(const TreeNode&) override;
    void writeToNode(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
