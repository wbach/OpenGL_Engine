#pragma once
#include <string>

#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/File.h"

namespace GameEngine
{
namespace Components
{
class ENGINE_API ItemVisualComponent : public BaseComponent
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

    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
