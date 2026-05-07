#pragma once

#include <memory>

#include "GameEngine/Components/ComponentCore.h"

class TreeNode;

namespace GameEngine
{
namespace Components
{
class UnknownExternalComponent : public ComponentCore
{
public:
    UnknownExternalComponent(ComponentContext&, GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    std::string GetOrginalComponentName() const ;

private:
    std::unique_ptr<TreeNode> orginalNode;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
