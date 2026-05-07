#pragma once
#include "GameEngine/Components/Component.h"

namespace GameEngine
{
namespace Components
{
DECLARE_COMPONENT(NewComponentTemplate)
{
public:
    // clang-format off
    BEGIN_FIELDS()
    END_FIELDS()
    // clang-format on

public:
    NewComponentTemplate(ComponentContext&, GameObject&);
    ~NewComponentTemplate() override;
    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
