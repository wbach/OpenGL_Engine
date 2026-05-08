#pragma once
#include <optional>
#include <string>

#include "GameEngine/Components/Component.h"
#include "GameEngine/Resources/File.h"
#include "TreeNode.h"

namespace GameEngine
{
namespace Components
{
DECLARE_COMPONENT(ItemVisualComponent)
{
public:
    File iconPath    = "";
    File modelPath   = "";
    File dropSound   = "";
    float modelScale = 1.0f;

    // clang-format off
    BEGIN_FIELDS()
        FIELD_FILE(iconPath)
        FIELD_FILE(modelPath)
        FIELD_FILE(dropSound)
        FIELD_FLOAT(modelScale)
    END_FIELDS()
    // clang-format on

public:
    ItemVisualComponent(ComponentContext&, GameObject&);
    ~ItemVisualComponent() override;

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;
    const std::optional<TreeNode>& getRendererComponentNode() const;

private:
    std::optional<TreeNode> rendererComponentNode;

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
