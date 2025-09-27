#pragma once

#include <memory>

#include "GameEngine/Components/BaseComponent.h"

class TreeNode;

namespace GameEngine
{
namespace Components
{
class UnknownExternalComponent : public BaseComponent
{
public:
    UnknownExternalComponent(ComponentContext&, GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;

    std::string GetOrginalComponentName() const ;

private:
    std::unique_ptr<TreeNode> orginalNode;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
